#ifndef DFRAMEWORK_CONTROLLER_LOOP_ITERATION_DATA_H
#define DFRAMEWORK_CONTROLLER_LOOP_ITERATION_DATA_H

namespace dfw {

//!< This structure is passed to each controller's loop method with timing information.

struct loop_iteration_data {
	float		delta,		//!< Delta step.
			timestep,	//!< Duration of the full timestep before drawing.
			remaning;	//!< Time left to consume for drawing.
	int		step;		//!< Iteration number.

	loop_iteration_data(float _d)
		:delta(_d), timestep(0.f), remaning(0.f), step(0) {

	}
};

}

#endif
