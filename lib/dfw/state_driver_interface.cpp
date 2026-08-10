#include <dfw/state_driver_interface.h>
#include <lm/log.h>
#include <chrono>
#include <sstream>


using namespace dfw;

state_driver_interface::state_driver_interface(int e)
	:states(e), ci(nullptr)
{

}

void state_driver_interface::start(
	dfw::kernel& _kernel
) {
	if(!controllers.size()){
		throw std::runtime_error("no controllers registered.");
	}

	if(!controllers.count(states.get_current())){
		throw std::runtime_error("initial controller is unregistered.");
	}

	if(!states.is_function_specified()) {
		throw new std::runtime_error("state_driver_interface::init failed the state_controller does not have a state validation function.");
	}

	ci=controllers[states.get_current()];
	ci->awake(_kernel.get_input(), -1); //-1, because this is the first controller to awake and there is no previous one!

	_kernel.get_controller_chrono().start();
	loop(_kernel);
	_kernel.get_controller_chrono().stop();

	lm::log(_kernel.get_log()).info()<<"controller logic ran for "<<_kernel.get_controller_chrono().get_seconds()<<" seconds"<<std::endl;
}

void state_driver_interface::register_controller(
	int _index,
	controller_interface& _controller
) {
	if(controllers.count(_index)){

		std::stringstream ss;
		ss<<"cannot register controller with id "<<_index<<", duplicate index, "<<controllers.count(_index)<<" found";
		throw std::runtime_error(ss.str());
	}

	controllers[_index]=&_controller;
	_controller.inject_state_controller(states);
	cvm.register_controller(_index, &_controller);
}

//TODO: Not really an interface if I provide base methods xD
void state_driver_interface::loop(
	dfw::kernel& _kernel
) {

	cvm.reserve();
	auto& fps_counter=_kernel.get_fps_counter();

	//Delta time is measured in doubles, as it is a small and precise value for
	//each step. To accumulate large values, it is better to use a tools::chrono.
	ldtools::tdelta 	delta_step=_kernel.get_delta_step(),
						produced_time=0.;
	auto& input_i=_kernel.get_input();
	loop_iteration_data lid(delta_step);

	do {
		common_pre_loop_input(input_i, delta_step);
		common_pre_loop_step(delta_step);

	//The elapsed time from the previous drawing is advanced in logic
	//in discrete steps. If the value is larger than delta_step,
	//delta step will be used. delta_step is a fixed value but the
	//fps_counter keeps the timed to be consumed. We will not enter
	//the loop if there's not enough time to consume...

		lid.step=0;
		if(produced_time > get_max_timestep()) {
			produced_time=get_max_timestep();
		}

		lid.timestep=produced_time;
		lid.remaning=lid.timestep;
		while(produced_time >= delta_step) {

			produced_time-=delta_step;
			lid.remaning=produced_time;

			//TODO: How about "loop_callback"? Perhaps I want to do something with it? we need another function like ci->input_loop(input_i()) that
			//defaults to loop or something so we can do ci->get_input_callback() ? input_i().loop_callback(*ci->get_input_callback()) : input_i().loop();

			input_i.tic();

			common_loop_input(input_i, delta_step);
			common_loop_step(delta_step);

			ci->loop(input_i, lid);
			++lid.step;

			if(ci->is_break_loop()) break;

			if(states.is_change()) {

				if(!ci->can_leave_state()) {

					states.cancel();
				}
				else {

					//current is the top of the stack, previous is the one before...
					prepare_state(states.get_current(), states.get_previous());
					break;
				}
			}
		}

		//State change confirmation... Do not draw!.
		if(states.is_change()) {

			int prev=states.get_previous();
			int next=states.get_current();
			if(0!=prev) {

				controllers[prev]->slumber(input_i, next);
			}

			ci=controllers[next];
			ci->awake(input_i, prev);
			states.confirm();
		}
		//Draw. We produce some time for the logic engine as we draw.
		else {
			fps_counter.begin_time_produce();

			cvm.clear();
			ci->request_draw(cvm);

			//TODO: PASS MORE SHIT, LIKE HOW MUCH IT TOOK TO DRAW THE PREVIOUS????
			cvm.draw(_kernel.get_screen(), fps_counter.get_frame_count());
			_kernel.get_screen().update();

			produced_time+=fps_counter.end_time_produce();
			fps_counter.loop_step();

			//Produced time must be at least equal to delta_step.
			if(produced_time < delta_step) {
				fps_counter.fill_until(produced_time, delta_step);
			}
		}
	}while(!ci->is_leave());
}
