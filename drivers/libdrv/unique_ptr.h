/*
 * Copyright (c) 2022-2026 Vadym Hrynchyshyn <vadimgrn@gmail.com>
 */

#pragma once

#include <libusbip/generic_handle_ex.h>
#include <wdm.h>

namespace usbip
{

template<ULONG PoolTag>
struct pool_ptr_tag
{
        enum { value = PoolTag };
};

template<ULONG PoolTag>
using unique_ptr_base = generic_handle<void*, pool_ptr_tag<PoolTag>, nullptr>;

template<ULONG PoolTag>
inline void close_handle(
        _In_ typename unique_ptr_base<PoolTag>::type ptr, _In_ pool_ptr_tag<PoolTag> tag) 
{
        if (ptr) {
                ExFreePoolWithTag(ptr, tag.value);
        }
}

} // namespace usbip


namespace libdrv
{

using usbip::swap;

struct uninitialized_t { explicit uninitialized_t() = default; };
inline constexpr uninitialized_t uninitialized;

template<ULONG PoolTag>
class unique_ptr : public usbip::unique_ptr_base<PoolTag>
{
public:
        enum { pooltag = PoolTag };
        using usbip::unique_ptr_base<PoolTag>::unique_ptr_base;

        unique_ptr(_In_ POOL_TYPE PoolType, _In_ SIZE_T NumberOfBytes) :
                unique_ptr(ExAllocatePoolZero(PoolType, NumberOfBytes, pooltag)) {}

        unique_ptr(_In_ const uninitialized_t&, _In_ POOL_TYPE PoolType, _In_ SIZE_T NumberOfBytes) :
                unique_ptr(ExAllocatePoolUninitialized(PoolType, NumberOfBytes, pooltag)) {}
};

} // namespace libdrv
