#pragma once

#include "state_controller.h"
#include "controller_view_manager.h"
#include "input.h"
#include "controller_interface_drawable.h"
#include "loop_iteration_data.h"

namespace dfw
{

//!Defines the interface for a controller.

//!Each controller is like a different state of the application, being "menu",
//!"configuration", "pause"... This allows for separation of concerns.
//!All controllers to be used by the application must be registered in the
//!state_driver. Controllers can communicate amongst themselves through
//!the state driver (which has direct access to public methods) and
//!signal broadcasters. Every single one is drawable. The interface is just
//!there to allow the controller_view_manager to focus on its job.
class controller_interface:
	public controller_interface_drawable

{
	public:

	//!Class constructor. Sets all internals to null.
	controller_interface()
				:states(nullptr), leave(false), break_loop(false) {
	}

	//!Virtual class destructor.
	virtual ~controller_interface() {
	}

	//!Will return true if the controller signals "leave", which will effectively
	//!end the main loop and begin application shutdown.
	bool 				is_leave() const {
		return leave;
	}

	//!Will return true if the controller signals "break loop", which will cut
	//!the consume delta time - draw loop without exiting the application.
	bool 				is_break_loop() const {
		return break_loop;
	}

	//!Sets the "leave" signal.
	void 				set_leave(bool v) {
		leave=v;
	}

	//!Sets the "break loop" signal.
	void 				set_break_loop(bool v) {
		break_loop=v;
	}

	//!Injects the state controller object. This is automatically done by the
	//!state_driver_interface so this controller can request a change to another
	//!state.
	void				inject_state_controller(state_controller& c) {
		states=&c;
	}

	//!Requests that this controller be drawn. This is a default implementation
	//!that can be overriden by controllers so they can request the drawing
	//!of other controllers or even the stacking of views.
	virtual void			request_draw(controller_view_manager& cvm) {
		cvm.add_ptr(this);
	}

	//!Signals that the state controller must change states, effectively
	//!changing the current controller.
	void				set_state(int v)  {

		if(!can_leave_state()) {

			return;
		}

		if(states==nullptr) {
			throw std::runtime_error("state_controller was not injected");
		}

		states->set(v);
	}

	//!Signals that the state controller must add a new state to the stack,
	//!which will take the first place.
	void				push_state(int v) {

		if(!can_leave_state()) {

			return;
		}

		if(states==nullptr) {
			throw std::runtime_error("state_controller was not injected");
		}

		states->push(v);
	}

	//!Signals that the previous controller in the stack should take the lead.
	void				pop_state() {

		if(!can_leave_state()) {

			return;
		}

		if(states==nullptr) {
			throw std::runtime_error("state_controller was not injected");
		}

		states->pop();
	}

	std::size_t				state_size() const {

		if(states==nullptr) {
			throw std::runtime_error("state_controller was not injected");
		}

		return states->size();

	}

	//!Will define the main loop of the controller. Will be called at least
	//!once before drawing. The loop_iteration_data object will contain information
	//!about the current iteration, consumed time, etc.
	virtual void 			loop(input&, const dfw::loop_iteration_data&)=0;

	//!Will define the drawing logic of this controller. It is strongly advised
	//!that drawing does not affect the internal controller logic.
	virtual void 			draw(ldv::screen&, int fps)=0;

	//!Will be called when this controller is brought to the front. The second
	//!argument indicates the previous controller, -1 if this is the first to
	//!be awaken.
	virtual void 			awake(input&, int)=0;

	//!Will be called before a new controller is called to the front. The
	//!second argument indicates the next controller.
	virtual void 			slumber(input&, int)=0;

	//!Will return true if an state change is authorized via "set_state". This
	//!can be useful to forbid changes and allow background processes to
	//!return to the controller.
	virtual bool			can_leave_state() const=0;

	private:

	state_controller *		states;	//!< Pointer to the state controller.
	bool 				leave,		//!< Leave flag.
						break_loop;	//!< Break loop flag.
};

}
