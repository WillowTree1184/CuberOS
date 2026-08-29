// src/Headers/Coaf/V1/Validator.hpp

// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 WillowTree1184 <xucx_2020@163.com>

// Based on COAF Specification V1 (docs/CoafSpecification/V1.md). Independent implementation.

#pragma once

#include "../Types.hpp"
#include "../IValidator.hpp"
#include "Structure.hpp"

namespace coaf
{
    namespace v1
    {
        class ImageValidator : public IValidator
        {
        private:
            char *imageBegin = nullptr;
            U64 imageSize = 0;
            Header *header = nullptr;
            MainTable *mainTable = nullptr;
            Segment *segments;

            /// @brief Checks if `offset` + `size` &lt; `limit`
            /// @param offset
            /// @param size
            /// @param limit
            /// @return `true` if `offset` + `size` &lt; `limit`, otherwise `false`.
            inline bool IsInBounds(U64 offset, U64 size, U64 limit)
            {
                if (size > limit)
                {
                    return false;
                }

                if (offset > limit - size)
                {
                    return false;
                }

                return true;
            }

            inline bool IsInBounds(U64 offset, U64 count, U64 itemSize, U64 limit)
            {
                if (count == 0 || itemSize == 0)
                {
                    return offset <= limit;
                }

                if (itemSize == 0 || count > limit / itemSize)
                {
                    return false;
                }

                return IsInBounds(offset, count * itemSize, limit);
            }

            bool ValidateHeader()
            {
                header = reinterpret_cast<Header *>(imageBegin);

                // Magic MUST be Magic::Image
                if (header->Magic != Magic::Image)
                {
                    return false;
                }

                // Version MUST be 1
                if (header->Version != 1)
                {
                    return false;
                }

                // MainTableOffset MUST be multiples of 8
                if (header->MainTableOffset % 8 != 0)
                {
                    return false;
                }

                return true;
            }

            bool ValidateMainTable()
            {
                if (!IsInBounds(header->MainTableOffset, sizeof(MainTable), imageSize))
                {
                    return false;
                }

                mainTable = reinterpret_cast<MainTable *>(imageBegin + header->MainTableOffset);

                auto validateTable = [&](U64 offset, U64 count, U64 itemSize, bool isAllowZeroCount = true) -> bool
                {
                    if (count == 0)
                    {
                        // When count = 0, offset MUST = 0
                        return offset == 0 && isAllowZeroCount;
                    }

                    // Otherwise, offset MUST != 0 and MUST be multiples of 8
                    if (offset == 0 || offset % 8 != 0)
                    {
                        return false;
                    }

                    // Check overflow ('cause offset + count * size MUST < imageSize)
                    if (!IsInBounds(offset, count, itemSize, imageSize))
                    {
                        return false;
                    }

                    return true;
                };

                // Validate SegmentTable
                if (!validateTable(mainTable->SegmentTableOffset, mainTable->SegmentCount, sizeof(Segment)))
                {
                    return false;
                }

                // Validate ExportSymbolTable
                if (!validateTable(mainTable->ExportSymbolTableOffset, mainTable->ExportSymbolCount, sizeof(ExportSymbol)))
                {
                    return false;
                }

                // Validate RelocationTableOffset
                if (!validateTable(mainTable->RelocationTableOffset, mainTable->RelocationCount, sizeof(U64)))
                {
                    return false;
                }

                // Validate RelocationTableOffset
                if (!validateTable(mainTable->StringTableOffset, mainTable->StringTableSize, 1))
                {
                    return false;
                }

                return true;
            }

            bool ValidateSegmentTable()
            {
                if (!IsInBounds(mainTable->SegmentTableOffset, mainTable->SegmentCount, sizeof(Segment), imageSize))
                {
                    return false;
                }

                // SegmentCount MUST > 0
                if (mainTable->SegmentCount <= 0)
                {
                    return false;
                }

                segments = reinterpret_cast<Segment *>(imageBegin + mainTable->SegmentTableOffset);

                U64 lastImageOffset = 0;
                for (U64 i = 0; i < mainTable->SegmentCount; i++)
                {
                    Segment current = segments[i];

                    // Validate permissions
                    if (!Any(current.Permissions, v1::Permission::Read))
                    {
                        return false;
                    }
                    if (Any(current.Permissions, v1::Permission::Write) && Any(current.Permissions, v1::Permission::Execute))
                    {
                        return false;
                    }
                    if ((U64)current.Permissions & ~(U64)(v1::Permission::Read | v1::Permission::Write | v1::Permission::Execute))
                    {
                        return false;
                    }

                    // MemorySize MUST > FileSize
                    if (current.MemorySize < current.FileSize)
                    {
                        return false;
                    }

                    // ImageOffset MUST be multiples of PageSize
                    if (current.ImageOffset % v1::PageSize != 0)
                    {
                        return false;
                    }

                    // MemorySize MUST > 0
                    if (current.MemorySize <= 0)
                    {
                        return false;
                    }

                    // In case of FileSize = 0, FileOffset MUST = 0 too
                    if (current.FileSize == 0 && current.FileOffset != 0)
                    {
                        return false;
                    }

                    // FileOffset + FileSize CAN NOT > imageSize
                    if (!IsInBounds(current.FileOffset, current.FileSize, imageSize))
                    {
                        return false;
                    }

                    // FileOffset + FileSize CAN NOT > mainTable->ImageSize
                    if (!IsInBounds(current.ImageOffset, current.MemorySize, mainTable->ImageSize))
                    {
                        return false;
                    }

                    // Find overlap
                    for (U64 j = 0; j < i; j++) // [WillowTree1184] O(n^2) TAT
                    {
                        Segment target = segments[j];

                        // Overlap is INVALID
                        U64 currentBegin = current.ImageOffset;
                        U64 currentEnd = current.ImageOffset + current.MemorySize;
                        U64 targetBegin = target.ImageOffset;
                        U64 targetEnd = target.ImageOffset + target.MemorySize;
                        if ((targetBegin >= currentBegin && targetBegin < currentEnd) ||
                            (targetEnd >= currentBegin && targetEnd < currentEnd) ||
                            (currentBegin >= targetBegin && currentBegin < targetEnd) ||
                            (currentEnd >= targetBegin && currentEnd < targetEnd))
                        {
                            return false;
                        }
                    }

                    // The segments MUST be sorted in increasing order according to ImageOffset
                    if (lastImageOffset > 0 && current.ImageOffset <= lastImageOffset)
                    {
                        return false;
                    }
                    lastImageOffset = current.ImageOffset;
                }

                // ImageSize must = the maximum of each segment's (ImageOffset + MemorySize), rounded up to the CoafPageSize.
                Segment last = segments[mainTable->SegmentCount - 1]; // [WillowTree1184] Since the segment table is already sorted, just take the last one.
                U64 upperBound = last.ImageOffset + last.MemorySize;
                if (mainTable->ImageSize != upperBound + upperBound % v1::PageSize)
                {
                    return false;
                }

                return true;
            }

        public:
            ImageValidator(char *imageBegin, U64 imageSize)
                : imageBegin(imageBegin),
                  imageSize(imageSize)
            {
            }

            bool Validate() override
            {
                if (!ValidateHeader())
                {
                    return false;
                }

                if (!ValidateMainTable())
                {
                    return false;
                }

                if (!ValidateSegmentTable())
                {
                    return false;
                }

                return true;
            }
        };

    } // namespace coaf::v1
}