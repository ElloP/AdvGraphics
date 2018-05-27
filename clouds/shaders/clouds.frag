#version 420

layout(location = 0) out vec4 fcolor;
layout(binding = 0) uniform sampler3D cloudTexture;

uniform vec3 view_port;
uniform vec3 camera_pos;
uniform vec3 camera_dir;
uniform float sun_dir;
uniform float time; 
uniform mat4 view;
uniform mat4 proj;
uniform mat4 inv_view;
uniform mat4 inv_proj;

uniform int distr;

uniform float cutoff;

float yOffset = 100.0f;
float cHeight = 64.0f;
float cWidth = 128.0f;
float steps = 100.0f;

float PI = 3.14159265;
float SQRT_OF_TWO = 1.41421356237;

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

bool facing_away(vec3 origin, vec3 dir){
	if(
		(origin.y < yOffset && sign(dir.y) < 0) ||
		(origin.y > (yOffset + cHeight) && sign(dir.y) > 0)
		){
		return true;
	}

	return false;
}

float sampleTexture(vec3 coord){
	vec4 sampA = texture(cloudTexture, coord);
	vec4 sampB = texture(cloudTexture, coord +  vec3(time,0.0f,0.0f));

	if(distr == 0){
		return sampB.a;
	}
	else{
		return max(0.7f * sampB.a - sampA.b * 0.3f, 0);
	}
}

vec3 calculate_distance_to_clouds(vec3 origin, vec3 dir){
	if(origin.y < yOffset){
		return dir * (((yOffset - origin.y) / dir.y)); 
	}
	else if(origin.y > (yOffset + cHeight)){
		return dir * (((yOffset + cHeight - origin.y) / dir.y));
	}
	else{
		return vec3(0);
	}
}

float calculate_occlusion(vec3 dir, float delt, float alpha, float maxt, float t, vec3 p){
	float x = 1.0f;
	for(float i = 0; i < (maxt - t); i += delt){
		vec3 q = p + i * delt * dir;
		vec3 texCoord = vec3(q.x / cWidth, (q.y - yOffset) / cHeight, q.z / cWidth);

		float v = sampleTexture(texCoord);
		if(v > cutoff){
			alpha += v;
			//c += samp;
		}
		x += 1.0f;
	}
	return alpha;
}

float calculate_light(vec3 sun, float delt, float color, vec3 p) {
	for(float i = 1; i < 5; i++){
		vec3 q = p + i * (delt/4.0f) * sun;
		vec3 texCoord = vec3(q.x / cWidth, (q.y - yOffset) / cHeight, q.z / cWidth);
		float v = sampleTexture(texCoord);
		if(v > cutoff){
			color *= 0.95f;
		//	c.r *= 0.98;
		//	c.b *= 0.98;
		//	c.g *= 0.98;//vec4(c.rgb * 0.98f, c.a);
		}
	}
	return color;
}

vec4 cast_ray(vec3 origin, vec3 dir, float l)
{
	float delt = 1;//l / steps;
	
	if(facing_away(origin, dir)){
		discard;
	}

	vec3 cloudDistance = calculate_distance_to_clouds(origin, dir);
 
	float smallt = delt / 5.0;
	float mint = rand(gl_FragCoord.xy) * delt;
	float maxt = 64.0f;//l;

	vec3 result = vec3(1.0f);

	vec3 sun = normalize(vec3(1.0f,1.0f,0.0f));
	
	float alpha = 0.5f;
	float shaded = 1.0f;
	for(float t = mint; t < maxt; t += delt)
	{
		vec3 p = origin + cloudDistance + dir * t;
		if(f(p))
		{
			

			float y = (p.y - yOffset) / cHeight;
			vec2 xz = p.xz / cWidth;
			vec3 texCoord = vec3(xz.x, y, xz.y);
			float v = sampleTexture(texCoord);
			if(v > cutoff) {
				alpha = calculate_occlusion(dir,delt, alpha, maxt, t, p);

				shaded = calculate_light(sun,delt,shaded,p);

				return vec4(result * shaded, max(min(alpha * 2,1), 0));  
			}
		}
	}
	discard;
	return vec4(0.0f);
}

void main()
{
	/* Calculate the ray */
	// http://antongerdelan.net/opengl/raycasting.html
	float x = 2.0 * (gl_FragCoord.x) / view_port.x - 1.0;
	float y = 2.0 * (gl_FragCoord.y) / view_port.y - 1.0;
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
	fcolor = temp;//vec4(1,1,1,1);//vec4(temp.xyz,min(temp.w*2, 1.0f));
}
