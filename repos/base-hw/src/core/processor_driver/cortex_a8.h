/*
 * \brief  Processor driver for core
 * \author Martin stein
 * \date   2011-11-03
 */

/*
 * Copyright (C) 2011-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _PROCESSOR_DRIVER__CORTEX_A8_H_
#define _PROCESSOR_DRIVER__CORTEX_A8_H_

/* core includes */
#include <processor_driver/arm_v7.h>

namespace Cortex_a8
{
	/**
	 * Part of processor state that is not switched on every mode transition
	 */
	class Processor_lazy_state { };

	/**
	 * Processor driver for core
	 */
	struct Processor_driver : Arm_v7::Processor_driver
	{
		/**
		 * Ensure that TLB insertions get applied
		 */
		static void tlb_insertions() { flush_tlb(); }

		/**
		 * Prepare for the proceeding of a user
		 */
		static void prepare_proceeding(Processor_lazy_state *,
		                               Processor_lazy_state *) { }

		/**
		 * Return wether to retry an undefined user instruction after this call
		 */
		bool retry_undefined_instr(Processor_lazy_state *) { return false; }

		/**
		 * The Cortex A8 processor cannot page table walk from level one cache.
		 * Therefore, as the page-tables lie in write-back cacheable memory we've
		 * to clean the corresponding cache-lines even when a page table entry is added
		 */
		static void translation_added(Genode::addr_t addr, Genode::size_t size)
		{
			/*
			 * only clean lines as core, the kernel adds entries
			 * before MMU and caches are enabled
			 */
			if (is_user()) Kernel::update_data_region(addr, size);
		}
	};
}


/******************************
 ** Arm_v7::Processor_driver **
 ******************************/

void Arm_v7::Processor_driver::finish_init_phys_kernel() { }

#endif /* _PROCESSOR_DRIVER__CORTEX_A8_H_ */

