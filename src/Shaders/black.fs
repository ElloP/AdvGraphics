#version 420 core

#define MAX_STEPS 255
#define MIN_DIST 0.0
#define MAX_DIST 100.0
#define EPSILON 0.0001

out vec4 FragColor;

float sphere(vec3 point, float radius)
{
	return length(point) - radius;
}

float scene(vec3 point) 
{
	return sphere(point,1.0);
}

float rayMarch(vec3 eye, vec3 dir, float start, float end)
{
	float depth = start;
	for(int i = 0; i < MAX_STEPS; i++)
	{
		float dist = scene(eye+depth*dir);
		if(dist < EPSILON) return depth;
		depth += dist;
		if(depth >= end) return end;
	}
	return end;
}

vec3 rayDir(float fov, vec2 size, vec2 fragCoord)
{
	vec2 xy = fragCoord - size / 2.0;
	float z = size.y / tan(radians(fov)/2.0);
	return normalize(vec3(xy,-z));
}

vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        scene(vec3(p.x + EPSILON, p.y, p.z)) - scene(vec3(p.x - EPSILON, p.y, p.z)),
        scene(vec3(p.x, p.y + EPSILON, p.z)) - scene(vec3(p.x, p.y - EPSILON, p.z)),
        scene(vec3(p.x, p.y, p.z  + EPSILON)) - scene(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));
    
    float dotLN = dot(L, N);
    float dotRV = dot(R, V);
    
    if (dotLN < 0.0) {
        // Light not visible from this point on the surface
        return vec3(0.0, 0.0, 0.0);
    } 
    
    if (dotRV < 0.0) {
        // Light reflection in opposite direction as viewer, apply only diffuse
        // component
        return lightIntensity * (k_d * dotLN);
    }
    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;
    
    vec3 light1Pos = vec3(4.0,
                          2.0,
                          4.0 );
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);
    
    vec3 light2Pos = vec3(2.0 * sin(0.37),
                          2.0 * cos(0.37),
                          2.0);
    vec3 light2Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light2Pos,
                                  light2Intensity);    
    return color;
}

void main()
{
	vec3 dir = rayDir(45.0, vec2(1920, 1080), gl_FragCoord.xy);
	vec3 eye = vec3(0.0,0.0,5.0);
	float dist = rayMarch(eye,dir,MIN_DIST,MAX_DIST);
	if(dist > MAX_DIST-EPSILON) 
	{
		FragColor = vec4(0.0);
		return;
	}

	vec3 p = eye + dist * dir;
    
    vec3 K_a = vec3(0.2, 0.2, 0.2);
    vec3 K_d = vec3(0.7, 0.2, 0.2);
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;
    
    vec3 color = phongIllumination(K_a, K_d, K_s, shininess, p, eye);
    

	FragColor = vec4(color,1.0);

}