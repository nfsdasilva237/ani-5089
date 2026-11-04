#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

constexpr double kPi = 3.14159265358979323846;
struct Q { double w,x,y,z; };
Q yaw(double deg) { double h=deg*kPi/360.; return {std::cos(h),0,std::sin(h),0}; }
Q mul(Q a,Q b) { return {a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z,a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w}; }
Q conj(Q q) { return {q.w,-q.x,-q.y,-q.z}; }
Q norm(Q q) { double n=std::sqrt(q.w*q.w+q.x*q.x+q.y*q.y+q.z*q.z); return {q.w/n,q.x/n,q.y/n,q.z/n}; }
double err(Q a,Q b) { Q d=norm(mul(b,conj(a))); return 2*std::atan2(std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z),std::abs(d.w))*180/kPi; }
double gesture(const std::string &name,double t) {
 if(name=="rapide") { if(t<=0)return 0; if(t>=.4)return 90; return 45*(1-std::cos(kPi*t/.4)); }
 if(name=="lent") { if(t<=0)return 0; if(t>=1.2)return 45; return 22.5*(1-std::cos(kPi*t/1.2)); }
 if(name=="demi-tour") { if(t<=0)return 0; if(t>=.25)return 180; return 90*(1-std::cos(kPi*t/.25)); }
 if(name=="arret") return t<.4 ? 180*t : 72.;
 return 0;
}
std::pair<double,double> evaluate(const std::string &name,double duration) {
 const double dt=1./72., horizon=dt; double sumV=0,sumA=0,maxV=0,maxA=0; size_t count=0;
 for(double t=2*dt;t<=duration+.2;t+=dt) {
  double p0=gesture(name,t-2*dt),p1=gesture(name,t-dt),p2=gesture(name,t);
  double v=(p2-p1)/dt; double a=((p2-p1)/dt-(p1-p0)/dt)/dt;
  double truth=gesture(name,t+horizon),vPred=p2+v*horizon,aPred=vPred+.5*a*horizon*horizon;
  double ev=std::abs(truth-vPred),ea=std::abs(truth-aPred);
  sumV+=ev; sumA+=ea; maxV=std::max(maxV,ev); maxA=std::max(maxA,ea); ++count;
 }
 std::printf(" %s %.3f deg (max %.3f), acceleration %.3f deg (max %.3f)",name.c_str(),sumV/count,maxV,sumA/count,maxA);
 return {sumV/count,sumA/count};
}
int main() {
 evaluate("rapide",.4); std::puts("");
 evaluate("lent",1.2); std::puts("");
 evaluate("demi-tour",.25); std::puts("");
 evaluate("arret",.6); std::puts("");
}
