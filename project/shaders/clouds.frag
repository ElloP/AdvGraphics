#version 420

layout(location = 0) out vec4 fcolor;

uniform sampler3D cloud_texture;

uniform vec3 view_port;
uniform vec3 camera_pos;
uniform vec3 camera_dir;
uniform vec3 sun_pos;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 inv_view;
uniform mat4 inv_proj;

float PI = 3.14159265;

float f(vec3 v)
{
	return 2.0f;
	//return sin(v.x)*sin(v.z);
}

float cast_ray(vec3 origin, vec3 dir)
{
	float delt = 0.1f;
	float mint = 0.001f;
	float maxt = 100.0f;
	for(float t = mint; t < maxt; t += delt)
	{
		vec3 p = origin + dir * t;
		if(p.y < f(p))
		{
			return t;//(t - 0.5f*delt);

		}
	}

	return 0.0f;
}

void main()
{
	/* Calculate the ray */
	// http://antongerdelan.net/opengl/raycasting.html
	float x = 2.0 * gl_FragCoord.x / view_port.x - 1.0;
	float y = 2.0 * gl_FragCoord.y / view_port.y - 1.0;
	vec2 ray_nds = vec2(x, y);
	vec4 ray_clip = vec4(ray_nds, -1.0, 1.0);
	vec4 ray_view = inv_proj * ray_clip;
	ray_view = vec4(ray_view.xy, -1.0, 0.0);
	vec3 ray_world = (inv_view * ray_view).xyz;
	ray_world = normalize(ray_world);

	float temp = cast_ray(camera_pos, camera_dir);

	//vec4 cloud_color = vec4(temp,temp,temp,255.0f);
	fcolor = vec4(vec3(temp),1.0f);
}
