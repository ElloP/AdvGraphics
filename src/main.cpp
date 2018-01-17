#include "Window.h"
#include "vector.h"

int main()
{
	vec4 x = vec4(1,2,3,4);
	vec4 y = vec4(4,5,2,7);

	vec4 z = x.normalize();
	y = x + z + y;

	return 0;
}