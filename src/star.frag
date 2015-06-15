
char const * STAR_FRAG_SRC = STRINGIFY_GLSL_SHADER(330 core,

in vec3 local_pos;
in vec3 world_normal;

uniform float time = 0.0;

const float pi = 3.14159265359;

layout(location = 0) out vec4 o_color;

vec4 mod289(vec4 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0; 
}

float mod289(float x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0; 
}

vec4 permute(vec4 x) {
	return mod289(((x * 34.0) + 1.0) * x);
}

float permute(float x) {
	return mod289(((x * 34.0) + 1.0) * x);
}

vec4 taylor_inv_sqrt(vec4 r) {
	return 1.79284291400159 - 0.85373472095314 * r;
}

float taylor_inv_sqrt(float r) {
	return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 grad4(float j, vec4 ip) {
	const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
	vec4 p;
	vec4 s;

	p.xyz = floor(fract(vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
	p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
	s = vec4(lessThan(p, vec4(0.0)));
	p.xyz += (s.xyz * 2.0 - 1.0) * s.www; 	

	return p;
}
						
float noise_4d(vec4 v) {
	const vec4 C = vec4(0.138196601125011, 0.276393202250021, 0.414589803375032, -0.447213595499958);

	vec4 i = floor(v + dot(v, vec4(0.309016994374947451)));
	vec4 x0 = v - i + dot(i, C.xxxx);

	vec4 i0;
	vec3 isX = step(x0.yzw, x0.xxx);
	vec3 isYZ = step(x0.zww, x0.yyz);
	i0.x = isX.x + isX.y + isX.z;
	i0.yzw = 1.0 - isX;
	i0.y += isYZ.x + isYZ.y;
	i0.zw += 1.0 - isYZ.xy;
	i0.z += isYZ.z;
	i0.w += 1.0 - isYZ.z;

	vec4 i3 = clamp(i0, 0.0, 1.0);
	vec4 i2 = clamp(i0-1.0, 0.0, 1.0);
	vec4 i1 = clamp(i0-2.0, 0.0, 1.0);

	vec4 x1 = x0 - i1 + C.xxxx;
	vec4 x2 = x0 - i2 + C.yyyy;
	vec4 x3 = x0 - i3 + C.zzzz;
	vec4 x4 = x0 + C.wwww;

	i = mod289(i);
	float j0 = permute(permute(permute(permute(i.w) + i.z) + i.y) + i.x);
	vec4 j1 = permute(permute(permute(permute(i.w + vec4(i1.w, i2.w, i3.w, 1.0)) + i.z + vec4(i1.z, i2.z, i3.z, 1.0)) + i.y + vec4(i1.y, i2.y, i3.y, 1.0)) + i.x + vec4(i1.x, i2.x, i3.x, 1.0));

	vec4 ip = vec4(1.0 / 294.0, 1.0 / 49.0, 1.0 / 7.0, 0.0);

	vec4 p0 = grad4(j0, ip);
	vec4 p1 = grad4(j1.x, ip);
	vec4 p2 = grad4(j1.y, ip);
	vec4 p3 = grad4(j1.z, ip);
	vec4 p4 = grad4(j1.w, ip);

	vec4 norm = taylor_inv_sqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	p4 *= taylor_inv_sqrt(dot(p4,p4));

	vec3 m0 = max(0.6 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);
	vec2 m1 = max(0.6 - vec2(dot(x3,x3), dot(x4,x4)), 0.0);
	m0 = m0 * m0;
	m1 = m1 * m1;
	return 49.0 * (dot(m0 * m0, vec3(dot(p0, x0), dot(p1, x1), dot(p2, x2))) + dot(m1 * m1, vec2(dot(p3, x3), dot(p4, x4))));
}

float cell_noise_3d(vec3 P)
{
    //	establish our grid cell and unit position
    vec3 Pi = floor(P);
    vec3 Pf = P - Pi;

    // clamp the domain
    Pi.xyz = Pi.xyz - floor(Pi.xyz * (1.0 / 69.0)) * 69.0;
    vec3 Pi_inc1 = step(Pi, vec3(69.0 - 1.5)) * (Pi + 1.0);

    // calculate the hash ( over -1.0->1.0 range )
    vec4 Pt = vec4(Pi.xy, Pi_inc1.xy) + vec2(50.0, 161.0).xyxy;
    Pt *= Pt;
    Pt = Pt.xzxz * Pt.yyww;
    const vec3 SOMELARGEFLOATS = vec3(635.298681, 682.357502, 668.926525);
    const vec3 ZINC = vec3(48.500388, 65.294118, 63.934599);
    vec3 lowz_mod = vec3(1.0 / (SOMELARGEFLOATS + Pi.zzz * ZINC));
    vec3 highz_mod = vec3(1.0 / (SOMELARGEFLOATS + Pi_inc1.zzz * ZINC));
    vec4 hash_x0 = fract(Pt * lowz_mod.xxxx) * 2.0 - 1.0;
    vec4 hash_x1 = fract(Pt * highz_mod.xxxx) * 2.0 - 1.0;
    vec4 hash_y0 = fract(Pt * lowz_mod.yyyy) * 2.0 - 1.0;
    vec4 hash_y1 = fract(Pt * highz_mod.yyyy) * 2.0 - 1.0;
    vec4 hash_z0 = fract(Pt * lowz_mod.zzzz) * 2.0 - 1.0;
    vec4 hash_z1 = fract(Pt * highz_mod.zzzz) * 2.0 - 1.0;

    //  generate the 8 point positions
    const float JITTER_WINDOW = 0.166666666;	// 0.166666666 will guarentee no artifacts.
    hash_x0 = ((hash_x0 * hash_x0 * hash_x0) - sign(hash_x0)) * JITTER_WINDOW + vec4(0.0, 1.0, 0.0, 1.0);
    hash_y0 = ((hash_y0 * hash_y0 * hash_y0) - sign(hash_y0)) * JITTER_WINDOW + vec4(0.0, 0.0, 1.0, 1.0);
    hash_x1 = ((hash_x1 * hash_x1 * hash_x1) - sign(hash_x1)) * JITTER_WINDOW + vec4(0.0, 1.0, 0.0, 1.0);
    hash_y1 = ((hash_y1 * hash_y1 * hash_y1) - sign(hash_y1)) * JITTER_WINDOW + vec4(0.0, 0.0, 1.0, 1.0);
    hash_z0 = ((hash_z0 * hash_z0 * hash_z0) - sign(hash_z0)) * JITTER_WINDOW + vec4(0.0, 0.0, 0.0, 0.0);
    hash_z1 = ((hash_z1 * hash_z1 * hash_z1) - sign(hash_z1)) * JITTER_WINDOW + vec4(1.0, 1.0, 1.0, 1.0);

    //	return the closest squared distance
    vec4 dx1 = Pf.xxxx - hash_x0;
    vec4 dy1 = Pf.yyyy - hash_y0;
    vec4 dz1 = Pf.zzzz - hash_z0;
    vec4 dx2 = Pf.xxxx - hash_x1;
    vec4 dy2 = Pf.yyyy - hash_y1;
    vec4 dz2 = Pf.zzzz - hash_z1;
    vec4 d1 = dx1 * dx1 + dy1 * dy1 + dz1 * dz1;
    vec4 d2 = dx2 * dx2 + dy2 * dy2 + dz2 * dz2;
    d1 = min(d1, d2);
    d1.xy = min(d1.xy, d1.wz);
    return min(d1.x, d1.y) * (9.0 / 12.0); // return a value scaled to 0.0->1.0
}

void main() {
	int s_o = 4;
	float s_p = 0.5;

	float n = 0.0;
	for(int i = 0; i < s_o; i++) {
		float r = float(i);
		float f = pow(2.0, r);
		float a = pow(s_p, r);

		n += noise_4d(vec4(local_pos * f, time * 0.5)) * a;
	}

	n = n * 0.5 + 0.5;
	float k = max(n, 0.5) * 8.0;

	int c_o = 4;
	float c_p = 1.0;

	float m = 0.0;
	for(int i = 0; i < c_o; i++) {
		float r = float(i);
		float f = pow(2.0, r);
		float a = pow(c_p, r);

		m += cell_noise_3d(local_pos * f * 8.0 + k) * a;
		// m += cell_noise_3d(local_pos * f * n);
	}

	m = max(m, n * 1.2);

	float t = mix(n, m, 0.25);
	float u = (1.0 - t);

	float t_2 = t * t;
	float u_2 = u * u;

	vec3 drk = vec3(0.8, 0.0, 0.0);
	vec3 med = vec3(1.0, 0.6, 0.1);
	vec3 lgt = vec3(1.0, 0.8, 0.2);

	// vec3 drk = vec3(0.0, 0.0, 0.8);
	// vec3 med = vec3(0.1, 0.6, 1.0);
	// vec3 lgt = vec3(0.2, 0.8, 1.0);

	vec3 color = drk * u_2 + med * (2.0 * u * t) + lgt * t_2;

	float edge = 1.0 - dot(vec3(0.0, 0.0, 1.0), world_normal);
	edge = pow(edge, 4.0) * 4.0;

	// color += vec3(1.0, 1.0, 0.6) * edge;

	o_color = vec4(color, 1.0);
}

);


