#version 420

layout(location = 0) out vec4 fcolor;


layout(binding = 0) uniform sampler3D cloudTexture;

uniform vec3 view_port;
uniform vec3 camera_pos;
uniform vec3 camera_dir;
uniform vec3 sun_pos;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 inv_view;
uniform mat4 inv_proj;

float yOffset = 100.0f;
float cHeight = 140.0f;
float cWidth = 200.0f;
float steps = 100.0f;

float PI = 3.14159265;

bool f(vec3 v)
{
	if (v.y > yOffset && v.y < (yOffset + cHeight)){
		return true;
	}
	return false;
	//return sin(v.x)*sin(v.z);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 cast_ray(vec3 origin, vec3 dir, float l)
{
	float delt = 10.0f;//l / steps;
	float smallt = delt / 5;
	float mint = 0.0001f + rand(gl_FragCoord.xy)*delt;
	float maxt = 1000.0f;//l;
	for(float t = mint; t < maxt; t += delt)
	{
		vec3 p = origin + dir * t;
		if(f(p))
		{
			float y = (p.y - yOffset) / cHeight;
			vec2 xz = p.xz / cWidth;
			vec3 texCoord = vec3(xz.x, y, xz.y);
			vec4 c = texture3D(cloudTexture,texCoord);
			if(c.w > 0.05f) {

				for(int i = 1; i < 10; i++){
					vec3 q = p + i * smallt * dir;
					float y = (q.y - yOffset) / cHeight;
					vec2 xz = q.xz / cWidth;
					vec3 texCoord = vec3(xz.x, y, xz.y);
					c += texture3D(cloudTexture,texCoord);
				}
				return c;  
			}
		}
	}
	return vec4(0.0f);
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


	//float imageAspectRatio = view_port.x / (float)view_port.y; // assuming width > height 
	//float Px = (2 * ((x + 0.5) / view_port.x) - 1) * tan(fov / 2 * PI / 180) * imageAspectRatio; 
	//float Py = (1 - 2 * ((y + 0.5) / view_port.y) * tan(fov / 2 * PI / 180); 
	float v = dot(camera_dir, ray_world);
	float vs = v / (length(camera_dir)*length(ray_world));
	float l = 1000.0 / vs;
	vec4 temp = cast_ray(camera_pos, ray_world, l);

	//vec4 cloud_color = vec4(temp,temp,temp,255.0f);
	fcolor = vec4(temp.xyz,min(temp.w*2, 1.0f));
}
