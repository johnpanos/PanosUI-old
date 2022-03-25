#include <vector>
#include <stdint.h>

namespace Wayland {
	class Output
	{
		int32_t width;
		int32_t height;
		uint8_t scale;

		Output();

		int32_t get_width();
		int32_t get_height();

		uint8_t get_scale();
	};

	class OutputManager
	{
		std::vector<Output *> outputs;

		OutputManager();
	};
};
