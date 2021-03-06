/*
 * \brief  Programmable interrupt controller for core
 * \author Stefan Kalkowski
 * \date   2012-10-24
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* core includes */
#include <pic.h>

using namespace Genode;

Pic::Pic() : Mmio(Board::TZIC_MMIO_BASE) { _common_init(); }


void Pic::unsecure(unsigned) { }


void Pic::secure(unsigned) { }
