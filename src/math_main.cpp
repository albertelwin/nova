
#include <cstdio>

struct { float value; bool success; } div_float(float x, float y) {
	if(y != 0.f) {
		return { x / y, true }; 
	}
	else {
		return { 0.f, false };
	}
}

int main() {
	auto print_div_result = [](float value, bool success) {
		if(success) {
			std::printf("LOG: %f\n", value);
		}
		else {
			std::printf("ERROR: Divide failed.\n");
		}
	};

	auto div_good = div_float(1.f, 2.f);
	auto div_bad = div_float(2.f, 0.f);

	print_div_result(div_good.value, div_good.success);
	print_div_result(div_bad.value, div_bad.success);

	return 0;
}