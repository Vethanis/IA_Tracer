#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;

layout(binding = 0, rg32f) uniform image2D dbuf;

layout(std140, binding=2) uniform CamBlock
{
	mat4 IVP;
	vec4 eye;
	vec4 nfp;	// near, far
	ivec4 whnp; // width, height, num_prims
};

#define EYE eye.xyz
#define NEAR nfp.x
#define FAR nfp.y
#define NPRIMS whnp.z
#define WIDTH whnp.x
#define HEIGHT whnp.y
#define MAX_DEPTH whnp.w

float invNear = 1.0f/NEAR;
float invFar = 1.0f/FAR;
float invfmn = invFar - invNear;
float expd = 1.0f / invfmn;
float toExp(float z){
	return (1./z - invNear) * expd;
}
float toLin(float f){
	return 1.0 / (f * invfmn + invNear);
}
float toNF(float l){
	return NEAR + l * (FAR - NEAR);
}

vec2 iadd(vec2 a, vec2 b){return a + b;}
vec2 iadd(vec2 a, float b){return a + b;}
vec2 iadd(float a, vec2 b){return a + b;}
vec2 isub(vec2 a, vec2 b){return a - b.yx;}
vec2 isub(vec2 a, float b){return a - b;}
vec2 isub(float a, vec2 b){return a - b.yx;}
vec2 imul(vec2 a, vec2 b){
	vec4 f = vec4(a.xxyy * b.xyxy);	
	return vec2(
		min(min(f[0],f[1]),min(f[2],f[3])),
		max(max(f[0],f[1]),max(f[2],f[3])));
}
vec2 imul(float a, vec2 b){
	vec2 f = vec2(a*b);	
	return vec2(
		min(f[0],f[1]),
		max(f[0],f[1]));
}
vec2 imul(vec2 b, float a){
	vec2 f = vec2(a*b);	
	return vec2(
		min(f[0],f[1]),
		max(f[0],f[1]));
}
vec2 ipow2(vec2 a){	
	return (a.x>=0.0)?vec2(a*a):(a.y<0.0)?vec2((a*a).yx):vec2(0.0,max(a.x*a.x,a.y*a.y));
}
vec2 ipow4(vec2 a){
	return (a.x>=0.0)?vec2(a*a*a*a):(a.y<0.0)?vec2((a*a*a*a).yx):vec2(0.0,max(a.x*a.x*a.x*a.x,a.y*a.y*a.y*a.y));
}
vec2 ipow6(vec2 a){
	return (a.x>=0.0)?vec2(a*a*a*a*a*a):(a.y<0.0)?vec2((a*a*a*a*a*a).yx):vec2(0.0,max(a.x*a.x*a.x*a.x*a.x*a.x,a.y*a.y*a.y*a.y*a.y*a.y));
}
vec2 imin(vec2 a, vec2 b){
	return vec2(min(a.x,b.x),min(a.y,b.y));
}
vec2 imax(vec2 a, vec2 b){
	return vec2(max(a.x,b.x),max(a.y,b.y));
}
vec3 imin(vec3 a, vec3 b){
	return vec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}
vec3 imax(vec3 a, vec3 b){
	return vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}
float imin(vec3 a){
	return min(min(a.x, a.y), a.z);
}
float imax(vec3 a){
	return max(max(a.x, a.y), a.z);
}
vec2 ix(vec3 a, vec3 b){
	return vec2(min(a.x, b.x), max(a.x, b.x));
}
vec2 iy(vec3 a, vec3 b){
	return vec2(min(a.y, b.y), max(a.y, b.y));
}
vec2 iz(vec3 a, vec3 b){
	return vec2(min(a.z, b.z), max(a.z, b.z));
}
vec2 iabs(vec2 a){
  if (a.x >= 0.0)
      return a;
  if (a.y <= 0.0) 
      return vec2(-a.y, -a.x);
  return vec2(0, max(-a.x, a.y));
}
vec2 iunion(vec2 a, vec2 b){
	return vec2(min(a.x, b.x), max(a.y, b.y));
}
vec2 iintersect(vec2 a, vec2 b){
	return vec2(max(a.x, b.x), min(a.y, b.y));
}
vec2 ismoothmin(vec2 a, vec2 b, float r){
	vec2 e = imin(vec2(r), imax(isub(vec2(r), iabs(isub(a, b))), vec2(0.0)));
	return isub(imin(a, b), imul(ipow2(e), 0.25/r));
}
vec2 ismoothmax(vec2 a, vec2 b, float r){
	vec2 e = imin(vec2(r), imax(isub(vec2(r), iabs(isub(a, b))), vec2(0.0)));
	return iadd(imax(a, b), imul(ipow2(e), 0.25/r));
}
vec4 imod2(vec2 a, float b){
	if ((a.y - a.x) >= b)
       		return vec4(0.0, b, 0.0, b);
	else {
            a = mod(a,b);
            if (a.y < a.x)
                return vec4(a.x,b,0.0,a.y);
                return vec4(a,a);
	}
}
vec2 isqrt(vec2 a){
	return vec2(sqrt(a.x), sqrt(a.y));
}
vec2 ilength(vec2 a, vec2 b){
	return isqrt(ipow2(a) + ipow2(b));
}
vec2 itri(vec2 x, float p){
	vec4 m = imod2(x + (p*0.5), p) - 0.5*p;
	return iunion(iabs(m.xy), iabs(m.zw));
}
vec2 itorus(vec2 x, vec2 y, vec2 z, vec2 t){
	return isub(ilength(isub(ilength(x, y), t.x), z), t.y);
}
vec2 isphere(vec2 x, vec2 y, vec2 z, vec3 c, float r){
	return ipow2(x-c.x) + ipow2(y-c.y) + ipow2(z-c.z) - r*r;
}
vec2 isphere2(vec2 x, vec2 y, vec2 z, vec3 c, float r){
	return ipow4(x-c.x) + ipow4(y-c.y) + ipow4(z-c.z) - r*r;
}
vec2 isphere3(vec2 x, vec2 y, vec2 z, vec3 c, float r){
	return ipow6(x-c.x) + ipow6(y-c.y) + ipow6(z-c.z) - r*r;
}
vec2 icube(vec2 x, vec2 y, vec2 z, float s){
	return isub(
		imax(
			imax(
				iabs(x), 
				iabs(y)),
			iabs(z)),
		s);
}
vec2 ibox(vec3 l, vec3 h, vec3 c, vec3 d){
	vec3 a = c - d;
	vec3 b = c + d;
	vec3 la = l - a;
	vec3 lb = l - b;
	vec3 ha = h - a;
	vec3 hb = h - b;
	return vec2(imax(imin(la, lb)), imin(imax(ha, hb)));
}
bool contains(vec2 a, float s){
	return a.x <= s && a.y >= s;
}
float width(vec2 a){
	return a.y - a.x;
}
float center(vec2 a){
	return 0.5*(a.x+a.y);
}
vec2 inear(vec2 a){
	return vec2(a.x, center(a));
}
vec2 ifar(vec2 a){
	return vec2(center(a), a.y);
}
vec2 ipop(vec2 a){
	float dd = center(a);
	return vec2(a.x+dd, a.y+2.0f*dd);
}
float maxabs(vec2 a){
	return max(abs(a.x), abs(a.y));
}

vec2 paniq_scene(vec2 a, vec2 b, vec2 c){
	vec2 d = itri(a, 40.0f);
	vec2 e = itri(b, 40.0f);
	vec2 f = itri(c, 40.0f);
	return imin(
		itorus(d, e, f, vec2(1.0f, 0.2f)),
		icube(d, e, f, 0.5f)
		);
}

vec2 l_scene(vec2 a, vec2 b, vec2 c){
	a = itri(a, 3.0f); b = itri(b, 4.0f); c = itri(c, 5.0f);
	return ismoothmin(
		isphere(a, b, c, vec3(1.0f), 1.0f),
		icube(a, b, c, 1.0f),
		1.0f);
}

vec2 map(vec3 a, vec3 b){
	vec2 c = ix(a, b); vec2 d = iy(a, b); vec2 e = iz(a, b);
	//return l_scene(c, d, e);
	return paniq_scene(c, d, e);
	//return isphere(c, d, e, vec3(0.f), 1.f);
	//return icube(c, d, e, 0.5f);
}

vec3 getPos(vec2 uv, float z){
	vec4 t = vec4(uv, z, 1.f);
	t = IVP * t;
	return vec3(t / t.w);
}

vec2 trace2(vec2 uv, vec2 t, float e){
	const int sz = 16;
	vec2 stack[sz];
	int end = 0;
	stack[end] = ifar(t);
	end++;
	stack[end] = inear(t);
	int entries = 2;
	for(int i = 0; i < 300; i++){
		vec2 cur = stack[end];
		end--; if(end < 0) end = sz-1;
		entries--;
		vec2 F = map(getPos(uv, cur.x), getPos(uv, cur.y));
		if(contains(F, 0.0f)){
			if(width(cur) < e) return cur;
			end = (end+1) % sz;
			stack[end] = ifar(cur);	 // push
			end = (end+1) % sz;
			stack[end] = inear(cur); // push
			entries = min(entries+2, sz);
			continue;
		}
		if(entries <= 0) break;
	}
	return vec2(1.0f);
}

void toInterval(vec2 u, vec2 v, vec2 t, inout vec3 l, inout vec3 h){
	vec3 d = getPos(vec2(u.x, v.x), t.x);
	{
		vec3 e = getPos(vec2(u.x, v.x), t.y);
		l = imin(d, e); h = imax(d, e);
	}
	d = getPos(vec2(u.x, v.y), t.x);
	l = imin(l, d); h = imax(h, d);
	d = getPos(vec2(u.x, v.y), t.y);
	l = imin(l, d); h = imax(h, d);
	d = getPos(vec2(u.y, v.x), t.x);
	l = imin(l, d); h = imax(h, d);
	d = getPos(vec2(u.y, v.x), t.y);
	l = imin(l, d); h = imax(h, d);
	d = getPos(vec2(u.y, v.y), t.x);
	l = imin(l, d); h = imax(h, d);
	d = getPos(vec2(u.y, v.y), t.y);
	l = imin(l, d); h = imax(h, d);
}

vec2 strace(vec2 u, vec2 v, vec2 t, float e){
	const int sz = 16;
	vec2 stack[sz];
	int end = 0;
	stack[end] = ifar(t);
	end++;
	stack[end] = inear(t);
	int entries = 2;
	for(int i = 0; i < 300; i++){
		vec2 cur = stack[end];	// pop
		end--; if(end < 0) end = sz-1;
		entries--;
		vec3 l, h;
		toInterval(u, v, cur, l, h);
		vec2 F = map(l, h);
		if(contains(F, 0.0f)){
			if(width(cur) < e) return cur;
			end = (end+1) % sz;
			stack[end] = ifar(cur);	 // push
			end = (end+1) % sz;
			stack[end] = inear(cur); // push
			entries = min(entries+2, sz);
			continue;
		}
		if(entries <= 0) break;
	}
	return vec2(1.0f);
}

void getUVs(out vec2 u, out vec2 v, ivec2 cr, int depth){
	int dim = (depth == 0) ? 1 : int(pow(2, depth)); 
	int w = WIDTH / dim; 
	int h = HEIGHT / dim;
	int c = cr.x / w; 
	int r = cr.y / h;
	float dif = 2.0 / dim; 
	u.x = -1.0 + c*dif;  
	u.y = -1.0 + c*dif + dif;
	v.x = -1.0 + r*dif;
	v.y = -1.0 + r*dif + dif;
}

vec2 subdivide(vec2 t, ivec2 cr, float e){
	vec2 u, v;
	for(int j = 0; j < MAX_DEPTH; j++){
		t.y = 1.0f;
		getUVs(u, v, cr, j);
		t = strace(u, v, t, e);
		if(t.y >= 1.0f) return vec2(1.0f);
		e = e * 0.5f;
	}
	return t;
}

#define UNIFORM

void main(){
	ivec2 pix = ivec2(gl_GlobalInvocationID.xy);  
	ivec2 size = imageSize(dbuf);
	if (pix.x >= size.x || pix.y >= size.y) return;
#ifdef UNIFORM
	vec2 uv = (vec2(pix) / vec2(size)) * 2.0f - 1.0f;
	vec2 F = trace2(uv, vec2(0.0f, 1.0f), 0.000005f);
	if(F.y >= 1.0f) return;
	imageStore(dbuf, pix, vec4(center(F)));
#else
	vec2 F = subdivide(vec2(0.0f, 1.0f), pix, 0.005f);
	if(F.y >= 1.0f) return;
	imageStore(dbuf, pix, vec4(center(F)));
#endif
}

