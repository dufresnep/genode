/*
 * \brief  Loader session interface
 * \author Christian Prochaska
 * \author Norman Feske
 * \date   2009-10-05
 */

/*
 * Copyright (C) 2009-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__LOADER_SESSION__LOADER_SESSION_H_
#define _INCLUDE__LOADER_SESSION__LOADER_SESSION_H_

#include <base/rpc.h>
#include <base/rpc_args.h>
#include <dataspace/capability.h>
#include <nitpicker_view/capability.h>
#include <base/signal.h>
#include <session/session.h>

namespace Loader {

	using namespace Genode;

	struct Session : Genode::Session
	{
		/*
		 * Resolve ambiguity of 'size_t' type when using 'loader_session.h'
		 * together with libc headers.
		 */
		typedef Genode::size_t size_t;

		/*
		 * Exception types
		 */
		struct Exception : Genode::Exception { };
		struct View_does_not_exist       : Exception { };
		struct Rom_module_does_not_exist : Exception { };

		/**
		 * Return argument of 'view_geometry()'
		 */
		struct View_geometry
		{
			int width, height;
			int buf_x, buf_y;

			View_geometry(): width(0), height(0), buf_x(0), buf_y() {}
			View_geometry(int w, int h, int x, int y): width(w), height(h), buf_x(x), buf_y(y) {}
		};

		typedef Genode::Rpc_in_buffer<64>  Name;
		typedef Genode::Rpc_in_buffer<128> Path;

		static const char *service_name() { return "Loader"; }

		virtual ~Session() { }

		/**
		 * Allocate dataspace to be used as ROM module by the loaded subsystem
		 *
		 * \param name  designated name of the ROM module
		 * \param size  size of ROM module
		 *
		 * \return Dataspace_capability  dataspace that contains the backing
		 *                               store of the ROM module
		 *
		 * The content of the dataspace is made visible to the loaded subsystem
		 * not before 'commit_rom_module' has been called. This two-step
		 * procedure enables the client to update the content of the ROM module
		 * during the lifetime of the session by subsequently calling dataspace
		 * with the same name as argument. Each time, a new dataspace is
		 * allocated but not yet presented to the loaded subsystem. When
		 * calling 'commit_rom_module', the most recently allocated dataspace
		 * becomes visible. The server frees intermediate dataspaces that are
		 * no longer used.
		 */
		virtual Dataspace_capability alloc_rom_module(Name const &name, size_t size) = 0;

		/**
		 * Expose ROM module to loaded subsystem
		 *
		 * \throw Rom_module_does_not_exist  if the ROM module name wasn't
		 *                                   allocated beforehand
		 */
		virtual void commit_rom_module(Name const &name) = 0;

		/**
		 * Define RAM quota assigned to the subsystem
		 *
		 * The quantum specified must be in the bounds of the quota attached
		 * the session. Note that RAM resources used for ROM modules are
		 * accounted, too. If ROM modules are modified at runtime by subsequent
		 * calls of 'alloc_rom_module', the resources needed for the respective
		 * ROM modules are doubled.
		 *
		 * If 'ram_quota' is not called prior calling 'start', all available
		 * session resources will be assigned to the subsystem.
		 */
		virtual void ram_quota(size_t quantum) = 0;

		/**
		 * Constrain size of the nitpicker buffer used by the subsystem
		 *
		 * Calling this function prior 'start()' enables the virtualization
		 * of the nitpicker session interface.
		 */
		virtual void constrain_geometry(int width, int height) = 0;

		/**
		 * Set the parent view of the subsystem's view.
		 *
		 * If 'parent_view' is not called prior calling 'start', the
		 * subsystem's view will not have a parent view.
		 */
		virtual void parent_view(Nitpicker::View_capability view) = 0;

		/**
		 * Register signal handler notified at creation time of the first view
		 */
		virtual void view_ready_sigh(Signal_context_capability sigh) = 0;

		/**
		 * Register signal handler notified when a failure occurs in the
		 * loaded subsystem.
		 *
		 * This signal is delivered if any child process of the subsystem
		 * produces an unresolvable page fault, exists, or triggers a CPU
		 * exception. (e.g., division by zero) For more information about
		 * the possible types of faults, please refer to the documentation of
		 * 'Rm_session::fault_handler' and 'Cpu_session::exception_handler'.
		 *
		 * This function should not be called after the 'start' function.
		 */
		virtual void fault_sigh(Signal_context_capability sigh) = 0;

		/**
		 * Start subsystem
		 *
		 * \throw Rom_module_does_not_exist  if the specified binary could
		 *                                   not obtained as ROM module
		 */
		virtual void start(Name const &binary, Name const &label = "",
		                   Native_pd_args const &pd_args = Native_pd_args()) = 0;

		/**
		 * Return first nitpicker view created by the loaded subsystem
		 *
		 * \throw View_does_not_exist
		 */
		virtual Nitpicker::View_capability view() = 0;

		/**
		 * Return view geometry as initialized by the loaded subsystem
		 *
		 * \throw View_does_not_exist
		 */
		virtual View_geometry view_geometry() = 0;


		/*******************
		 ** RPC interface **
		 *******************/

		GENODE_RPC(Rpc_alloc_rom_module, Dataspace_capability, alloc_rom_module,
		                                 Name const &, size_t);
		GENODE_RPC_THROW(Rpc_commit_rom_module, void, commit_rom_module,
		                 GENODE_TYPE_LIST(Rom_module_does_not_exist),
		                 Name const &);
		GENODE_RPC(Rpc_ram_quota, void, ram_quota, size_t);
		GENODE_RPC(Rpc_constrain_geometry, void, constrain_geometry, int, int);
		GENODE_RPC(Rpc_parent_view, void, parent_view, Nitpicker::View_capability);
		GENODE_RPC(Rpc_view_ready_sigh, void, view_ready_sigh, Signal_context_capability);
		GENODE_RPC(Rpc_fault_sigh, void, fault_sigh, Signal_context_capability);
		GENODE_RPC_THROW(Rpc_start, void, start,
		                 GENODE_TYPE_LIST(Rom_module_does_not_exist),
		                 Name const &, Name const &, Native_pd_args const &);
		GENODE_RPC_THROW(Rpc_view, Nitpicker::View_capability, view,
		                 GENODE_TYPE_LIST(View_does_not_exist));
		GENODE_RPC(Rpc_view_geometry, View_geometry, view_geometry);

		/*
		 * 'GENODE_RPC_INTERFACE' declaration done manually
		 *
		 * The number of RPC function of this interface exceeds the maximum
		 * number of elements supported by 'Meta::Type_list'. Therefore, we
		 * construct the type list by hand using nested type tuples instead
		 * of employing the convenience macro 'GENODE_RPC_INTERFACE'.
		 */
		typedef Meta::Type_tuple<Rpc_alloc_rom_module,
			    Meta::Type_tuple<Rpc_commit_rom_module,
			    Meta::Type_tuple<Rpc_ram_quota,
			    Meta::Type_tuple<Rpc_constrain_geometry,
			    Meta::Type_tuple<Rpc_parent_view,
			    Meta::Type_tuple<Rpc_view_ready_sigh,
			    Meta::Type_tuple<Rpc_fault_sigh,
			    Meta::Type_tuple<Rpc_start,
			    Meta::Type_tuple<Rpc_view,
			    Meta::Type_tuple<Rpc_view_geometry,
			                     Meta::Empty>
			    > > > > > > > > > Rpc_functions;
	};
}

#endif /* _INCLUDE__LOADER_SESSION__LOADER_SESSION_H_ */
