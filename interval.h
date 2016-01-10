#ifndef INTERVAL_H
#define INTERVAL_H

#define GLM_SWIZZLE
#include "glm/glm.hpp"

/******************************************************************/
/*                                                                */
/* taken from nimitz and paniq: http://glslsandbox.com/e#29925.2  */
/*                                                                */
/******************************************************************/

namespace glm{

inline mat2 mm2(float a){float c = cos(a);float s = sin(a);return mat2(c,-s,s,c);}

inline mat3 iavec3(vec2 x, vec2 y, vec2 z) {
	return mat3(vec3(x.xy(), 0.0f), vec3(y.xy(), 0.0f), vec3(z.xy(), 0.0f));
}

inline mat3 iavec3(const vec3& p) {
	return mat3(vec3(p.x, p.x, 0.0f), vec3(p.x, p.x, 0.0f), vec3(p.z, p.z, 0.0f));
}

inline vec2 imin(vec2 a, vec2 b) {
	return vec2(min(a.x,b.x),min(a.y,b.y));
}

inline vec2 imax(vec2 a, vec2 b) {
	return vec2(max(a.x,b.x),max(a.y,b.y));
}

inline vec2 iadd(vec2 a, vec2 b) {
	return a + b;
}

inline mat3 iadd(const mat3& a, const mat3& b) {
	return iavec3(
		a[0].xy() + b[0].xy(),
		a[1].xy() + b[1].xy(),
		a[2].xy() + b[2].xy());
}

inline mat3 iadd(const vec3& a, const mat3& b) {
	return iavec3(
		a.xx() + b[0].xy(),
		a.yy() + b[1].xy(),
		a.zz() + b[2].xy());
}

inline vec2 isub(vec2 a, vec2 b) {
	return a - b.yx();
}

inline mat3 isub(const mat3& a, const mat3& b) {
	return iavec3(
		a[0].xy() - b[0].yx(),
		a[1].xy() - b[1].yx(),
		a[2].xy() - b[2].yx());
}

inline mat3 isub(const mat3& a, const vec3& b) {
	return iavec3(
		a[0].xy() - b.xx(),
		a[1].xy() - b.yy(),
		a[2].xy() - b.zz());
}

inline vec2 imul(vec2 a, float c) {
	vec2 b = vec2(c);
	vec4 f = vec4(
		a.xxyy() * b.xyxy()
	);	
	return vec2(
		min(min(f[0],f[1]),min(f[2],f[3])),
		max(max(f[0],f[1]),max(f[2],f[3])));
}

inline vec2 imul(vec2 a, vec2 b) {
	vec4 f = vec4(
		a.xxyy() * b.xyxy()
	);	
	return vec2(
		min(min(f[0],f[1]),min(f[2],f[3])),
		max(max(f[0],f[1]),max(f[2],f[3])));
}

inline vec2 imul(float a, vec2 b) {
	vec2 f = vec2(a*b);	
	return vec2(
		min(f[0],f[1]),
		max(f[0],f[1]));
}

inline mat3 imul(const mat3& a, const mat3& b) {
	return iavec3(
		imul(a[0].xy(), b[0].xy()),
		imul(a[1].xy(), b[1].xy()),
		imul(a[2].xy(), b[2].xy())
	);
}

inline mat3 imul(float a, const mat3& b) {
	return iavec3(
		imul(a, b[0].xy()),
		imul(a, b[1].xy()),
		imul(a, b[2].xy())
	);
}

inline mat3 imul(const vec3& a, const mat3& b) {
	return iavec3(
		imul(a.xx(), b[0].xy()),
		imul(a.yy(), b[1].xy()),
		imul(a.zz(), b[2].xy())
	);
}

inline mat3 imul(const vec3& a, vec2 b) {
	return iavec3(
		imul(a.x, b),
		imul(a.y, b),
		imul(a.z, b)
	);
}


inline vec2 idiv(vec2 a, vec2 b) {
	vec4 f = vec4(
		a.x/b, a.y/b
	);
	return vec2(
		min(min(f[0],f[1]),min(f[2],f[3])),
		max(max(f[0],f[1]),max(f[2],f[3])));
}

inline mat3 idiv(const mat3& a, const mat3& b) {
	return iavec3(
		idiv(a[0].xy(), b[0].xy()),
		idiv(a[1].xy(), b[1].xy()),
		idiv(a[2].xy(), b[2].xy())
	);
}

inline vec2 isqrt(vec2 a) {
	return vec2(sqrt(a.x),sqrt(a.y));
}

inline vec2 ipow2(vec2 a) {	
	return (a.x>=0.0f)?vec2(a*a):(a.y<0.0f)?vec2((a*a).yx()):vec2(0.0f,max(a.x*a.x,a.y*a.y));
}

inline mat3 ipow2(const mat3& v) {
	return iavec3(
		ipow2(v[0].xy()),
		ipow2(v[1].xy()),
		ipow2(v[2].xy()));
}

inline vec2 ilensq(const mat3& a) {
	mat3 c = ipow2(a);
	return c[0].xy() + c[1].xy() + c[2].xy();
}

inline vec2 ilength(const mat3& a) {
	mat3 c = ipow2(a);
	return isqrt(c[0].xy() + c[1].xy() + c[2].xy());
}

inline vec2 idot(const mat3& a, const mat3& b) {
	mat3 c = imul(a,b);
	return c[0].xy() + c[1].xy() + c[2].xy();
}

inline bool icontains(vec2 a, float v) {
	return ((v >= a.x) && (v < a.y));
}

inline vec2 iabs(vec2 a){
  if (a.x >= 0.0f)
      return a;
  if (a.y <= 0.0f) 
      return vec2(-a.y, -a.x);
  return vec2(0, max(-a.x, a.y));
}

inline vec2 ismoothmin(vec2 a, vec2 b, float r){
	vec2 e = imin(vec2(r), imax(isub(vec2(r), iabs(isub(a, b))), vec2(0.0f)));
	return isub(imin(a, b), imul(ipow2(e), 0.25f/r));
}

inline vec2 ismoothmax(vec2 a, vec2 b, float r){
	vec2 e = imin(vec2(r), imax(isub(vec2(r), iabs(isub(a, b))), vec2(0.0f)));
	return iadd(imax(a, b), imul(ipow2(e), 0.25f/r));
}
inline mat3 icross(const mat3& a, const vec3& b){
    return iavec3(
        isub(imul(b.z, a[1].xy()), imul(b.y, a[2].xy())),
	    isub(imul(b.x, a[2].xy()), imul(b.z, a[0].xy())),
        isub(imul(b.y, a[0].xy()), imul(b.x, a[1].xy()))
    );
}

inline mat3 icross(const vec3& a, const mat3& b){
    return iavec3(
        isub(imul(a.y, b[2].xy()), imul(a.z, b[1].xy())),
	    isub(imul(a.z, b[0].xy()), imul(a.x, b[2].xy())),
        isub(imul(a.x, b[1].xy()), imul(a.y, b[0].xy()))
    );
}

inline mat3 icross(const mat3& a, const mat3& b){
    return iavec3(
        isub(imul(a[1].xy(), b[2].xy()), imul(a[2].xy(), b[1].xy())),
	    isub(imul(a[2].xy(), b[0].xy()), imul(a[0].xy(), b[2].xy())),
        isub(imul(a[0].xy(), b[1].xy()), imul(a[1].xy(), b[0].xy()))
    );
}

inline mat3 irotateQuat(const vec4& q, const mat3& v){
    mat3 uv = icross(q.xyz(), v);
    mat3 uuv = icross(q.xyz(), uv);
    //v + ((uv * q.w) + uuv) * static_cast<T>(2);
    return iadd(v, imul(2.0f, iadd(imul(q.w, uv), uuv)));
}

inline vec4 quatNormalize(const vec4& q){
	return q/length(q);
}

inline vec4 angleAxis(float a, const vec3& v){
	return vec4(v * sin(a * 0.5f), cos(a * 0.5f));
}


inline vec2 isdBox(const mat3& p, const vec3& b){
  vec2 dx = isub(iabs(p[0].xy()), vec2(b.x));
  vec2 dy = isub(iabs(p[1].xy()), vec2(b.y));
  vec2 dz = isub(iabs(p[2].xy()), vec2(b.z));
  
  return imax(dx, imax(dy, dz));
}

// split interval into two intervals
inline vec4 imod2 (vec2 a, float b) {
	if ((a.y - a.x) >= b)
       		return vec4(0.0f, b, 0.0f, b);
	else {
            a = mod(a,b);
            if (a.y < a.x)
                return vec4(a.x,b,0.0f,a.y);
                return vec4(a,a);
	}
}

inline vec2 iunion(vec2 a, vec2 b) {
	return vec2(min(a.x,b.x),max(a.y,b.y));
}

inline vec2 tri(vec2 x) {
	vec4 m = imod2(x, 1.0f) - 0.5f;
	return iunion(iabs(m.xy()),iabs(m.zw()));
}

#define PI 3.1415926535898f

inline vec2 icos (vec2 a) {
	if ((a.y - a.x) >= (PI * 2.0f))
	        return vec2(-1.0f,1.0f);
	else {
		vec4 mma = imod2(a, PI * 2.0f) - PI;
		vec2 ma = isub(vec2(PI), iunion(iabs(mma.xy()),iabs(mma.zw())));
		vec2 ca = cos(ma);
		return vec2(
			icontains(ma, PI)?-1.0f:min(ca.x,ca.y),
			icontains(ma, 0.0f)?1.0f:max(ca.x,ca.y));
	}
}

inline vec2 ipow4(vec2 a){
	return ipow2(ipow2(a));	
}

inline vec2 ipow8(vec2 a){
	return ipow2(ipow4(a));	
}

inline vec2 ipow16(vec2 a){
	return ipow2(ipow8(a));	
}

inline vec2 ipow32(vec2 a){
	return ipow2(ipow16(a));	
}

inline vec2 ipow64(vec2 a){
	return ipow2(ipow32(a));
}

inline vec2 ipow128(vec2 a){
	return ipow2(ipow64(a));
}

inline vec2 isphere(const mat3& p, const vec3& c, float r){
	return ipow2(p[0].xy() + c.xx()) + ipow2(p[1].xy() - c.yy()) + ipow2(p[2].xy() - c.zz()) - r*r;
}

inline vec2 isphere2(const mat3& p, const vec3& c, float r){
	return ipow4(p[0].xy() + c.xx()) + ipow4(p[1].xy() - c.yy()) + ipow4(p[2].xy() - c.zz()) - r*r;
}

inline vec2 isphere4(const mat3& p, const vec3& c, float r){
	return ipow8(p[0].xy() + c.xx()) + ipow8(p[1].xy() - c.yy()) + ipow8(p[2].xy() - c.zz()) - r*r;
}

inline vec2 isphere8(const mat3& p, const vec3& c, float r){
	return ipow16(p[0].xy() + c.xx()) + ipow16(p[1].xy() - c.yy()) + ipow16(p[2].xy() - c.zz()) - r*r;
}

inline vec2 isphere16(const mat3& p, const vec3& c, float r){
	return ipow32(p[0].xy() + c.xx()) + ipow32(p[1].xy() - c.yy()) + ipow32(p[2].xy() - c.zz()) - r*r;
}

inline vec2 isphere32(const mat3& p, const vec3& c, float r){
	return ipow64(p[0].xy() + c.xx()) + ipow64(p[1].xy() - c.yy()) + ipow64(p[2].xy() - c.zz()) - r*r;
}

inline vec2 isphere64(const mat3& p, const vec3& c, float r){
	return ipow128(p[0].xy() + c.xx()) + ipow128(p[1].xy() - c.yy()) + ipow128(p[2].xy() - c.zz()) - r*r;
}

float icenter(vec2 v){
	return (v.x + v.y) * 0.5f;
}



/*

inline vec3 getNormal(const vec3& p){
	vec2 ep = vec2(0.0, 0.001);
	vec2 hx = map(iavec3(p + ep.yxx())) - map(iavec3(p - ep.yxx()));
	vec2 hy = map(iavec3(p + ep.xyx())) - map(iavec3(p - ep.xyx()));
	vec2 hz = map(iavec3(p + ep.xxy())) - map(iavec3(p - ep.xxy()));
	return normalize(vec3(hx.x, hy.x, hz.x));
}

inline vec2 trace(const vec3& ro, const vec3& rd, vec2 t) {
	mat3 r = iadd(ro, imul(rd, t));	
	return map(r);
}

inline vec3 trace(const vec3& ro, const vec3& rd){
	float e = 0.01f;
	const float maxt = 10.f;
	
	vec2 t = vec2(2.f, maxt);
	bool hit = false;
	
	for (int i = 0; i < 60; ++i){
		float th = (t.x+t.y)*.5f;
		
		vec2 t0 = vec2(t.x, th);
		vec2 t1 = vec2(th, t.y);
		
		vec2 d0 = trace(ro, rd, t0);
		vec2 d1 = trace(ro, rd, t1);
		
		if (icontains(d0, 0.0f)){
			t.y = th;
			if (max(abs(d0.y),abs(d0.x)) < e){
				hit = true;
				break;
			}
		}
		else if (icontains(d1, 0.0f)){
			t.x = th;
			if (max(abs(d0.y),abs(d0.x)) < e){
				hit = true;
				break;
			}
		}
		else{
			float d = (t.y-t.x);
			t.x += d;
			t.y += d*2.0f;
			if (t.x >= maxt) break;
		}
	}
	
	vec3 col = vec3(0.0f);
	
	if (hit) {
		float th = (t.x+t.y)*0.5f;
		vec3 p = (ro+rd*th);
		vec3 n = normal(p);
		//vec3 ref = reflect(rd,n);
		
		vec3 base = vec3(0.9f,.5f,.35f)*0.3f+0.7f;
		col = n * 0.5f + 0.5f;
        //col  = mix(col,pow(textureCube(iChannel1,ref).rgb,vec3(.9)),.5);
		col *= base;
        
        //vec3 lin = mix(vec3(0),vec3(1),n.y*0.4+0.3);
        //col = vec3(0.9,.5,.35)*0.4+0.6;
	}
	
	return col;
}


vec2 map(mat3 p) {
	p = irotateQuat(angleAxis(PI * 0.3, vec3(1.0,0.0,0.0)), p);
	vec2 x = isqrt(iadd(ipow2(p[0].xy),ipow2(p[1].xy)));
	vec2 d = iadd(imul(tri(imul(x - time * 0.5,5.0)),0.2),imul(icos(imul(x - time * 0.1,2.0)),0.5));
	vec2 w = tri(p[1].xy * 2.0) - 0.3;
	
	return imax(-w.yx,iabs(isub(p[2].xy, d)) - 0.02);
}

void main(){
	vec2 p = gl_FragCoord.xy / resolution.xy-0.5;
	float asp = resolution.x/resolution.y;
	p.x *= asp;
	
	//camera
	vec3 ro = vec3(0.,0.,4.);
	//ro.xz *= mm2(time*0.4);
	//ro.xy *= mm2(sin(time*0.1));
	vec3 ta = vec3(0.0, .0, 0.0);
    vec3 ww = normalize( ta - ro);
    vec3 uu = normalize(cross( vec3(0.0,1.0,0.0), ww ));
    vec3 vv = normalize(cross(ww,uu));
    vec3 rd = normalize( p.x*uu + p.y*vv + 1.4*ww );
	
	vec3 col = trace(ro,rd);
	gl_FragColor = vec4(col,1.0);
}
*/
};

#endif
