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
 if(name.find("rapide")==0) { if(t<=0)return 0; if(t>=.4)return 90; return 45*(1-std::cos(kPi*t/.4)); }
 if(name.find("lent")==0) { if(t<=0)return 0; if(t>=1.2)return 45; return 22.5*(1-std::cos(kPi*t/1.2)); }
 if(name.find("demi-tour")==0) { if(t<=0)return 0; if(t>=.25)return 180; return 90*(1-std::cos(kPi*t/.25)); }
 if(name=="arret") return t<.4 ? 180*t : 72.;
 return 0;
}
std::pair<double,double> evaluate(const std::string &name,double duration) {
	const double dt=1./72., horizon=dt;
	double sumPrediction=0., sumSans=0., maxPrediction=0., maxSans=0.;
	size_t count=0;
	for(double t=2*dt;t<=duration;t+=dt) {
		const double p1=gesture(name,t-dt), p2=gesture(name,t);
		const double vitesse=(p2-p1)/dt;
		const Q mesure=yaw(p2);
		const Q vraie=yaw(gesture(name,t+horizon));
		const Q predite=norm(mul(yaw(vitesse*horizon), mesure));
		const double erreurPrediction=err(predite,vraie);
		const double erreurSans=err(mesure,vraie);
		sumPrediction+=erreurPrediction; sumSans+=erreurSans;
		maxPrediction=std::max(maxPrediction,erreurPrediction);
		maxSans=std::max(maxSans,erreurSans); ++count;
	}
	const double moyennePrediction=sumPrediction/count, moyenneSans=sumSans/count;
	const double rapport=moyennePrediction>0. ? moyenneSans/moyennePrediction : 0.;
	std::printf("%s : avec %.3f deg, sans %.3f deg, rapport %.3f\n",
		name.c_str(), moyennePrediction, moyenneSans, rapport);
	std::printf("  maximum : avec %.3f deg, sans %.3f deg\n", maxPrediction, maxSans);
	return {moyennePrediction,moyenneSans};
}
int main() {
	evaluate("rapide (90 deg en 400 ms)",.4);
	evaluate("lent (45 deg en 1,2 s)",1.2);
	evaluate("demi-tour (180 deg en 250 ms)",.25);
}
