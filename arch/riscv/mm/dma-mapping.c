// SPDX-License-Identifier: GPL-2.0
/* T-HEAD C906 DMA cache maintenance, ported from the vendor 5.10 tree.
 * Coherent rings use DMA_DIRECT_REMAP + pgprot_noncached; streaming maps
 * call these sync helpers. Do not return a cacheable linear alias from
 * dma_alloc_coherent (no arch_dma_alloc).
 */

#include <linux/dma-map-ops.h>
#include <linux/export.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/types.h>
#include <asm/cache.h>
#include <asm/cacheflush.h>

void arch_dma_prep_coherent(struct page *page, size_t size)
{
	void *ptr = page_address(page);

	memset(ptr, 0, size);
	dma_wbinv_range(page_to_phys(page), page_to_phys(page) + size);
}
EXPORT_SYMBOL(arch_dma_prep_coherent);

static inline void cache_op(phys_addr_t paddr, size_t size,
			    void (*fn)(unsigned long start, unsigned long end))
{
	unsigned long start = (unsigned long)paddr;

	fn(start, start + size);
}

void arch_sync_dma_for_device(phys_addr_t paddr, size_t size,
			      enum dma_data_direction dir)
{
	switch (dir) {
	case DMA_TO_DEVICE:
		cache_op(paddr, size, dma_wb_range);
		break;
	case DMA_FROM_DEVICE:
	case DMA_BIDIRECTIONAL:
		cache_op(paddr, size, dma_wbinv_range);
		break;
	default:
		BUG();
	}
}
EXPORT_SYMBOL(arch_sync_dma_for_device);

void arch_sync_dma_for_cpu(phys_addr_t paddr, size_t size,
			   enum dma_data_direction dir)
{
	switch (dir) {
	case DMA_TO_DEVICE:
		return;
	case DMA_FROM_DEVICE:
	case DMA_BIDIRECTIONAL:
		cache_op(paddr, size, dma_wbinv_range);
		break;
	default:
		BUG();
	}
}
EXPORT_SYMBOL(arch_sync_dma_for_cpu);
