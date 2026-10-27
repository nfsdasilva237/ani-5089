#include <cmath>
#include <cstdio>
#include <algorithm>
#include "NKMath/NkMat.h"
#include "NKRenderer/Core/NkCamera.h"
using namespace nkentseu;
using namespace nkentseu::math;
using namespace nkentseu::renderer;
struct M4{float mat[4][4]{};};
static M4 Projection(float l,float r,float u,float d,float n,float f){
 const float tl=std::tan(l),tr=std::tan(r),tu=std::tan(u),td=std::tan(d),w=tr-tl,h=tu-td;
 M4 m{};m.mat[0][0]=2/w;m.mat[1][1]=2/h;m.mat[2][0]=(tr+tl)/w;m.mat[2][1]=(tu+td)/h;
 m.mat[2][2]=(f+n)/(n-f);m.mat[2][3]=-1;m.mat[3][2]=2*f*n/(n-f);return m;
}
static void print(const char*label,M4 a,M4 b){float maxe=0;
 for(int row=0;row<4;row++){float e[4];for(int col=0;col<4;col++){e[col]=std::fabs(a.mat[col][row]-b.mat[col][row]);maxe=std::max(maxe,e[col]);}
  std::printf("%s, ligne %d : %.7g %.7g %.7g %.7g\n",label,row,e[0],e[1],e[2],e[3]);}std::printf("%s : plus grand ecart %.7g\n",label,maxe);}
int main(){const float rad=3.14159265358979323846f/180.f,n=.05f,f=100.f;
 // Perspective symétrique: vertical 70°, horizontal déduit du ratio 1280/720.
 const float top=35*rad, right=std::atan(std::tan(top)*(1280.f/720.f));
 M4 sym=Projection(-right,right,top,-top,n,f);
 // Valeurs équivalentes du constructeur perspective, données ici pour comparaison.
 M4 persp{};const NkMat4f mp=NkMat4f::Perspective(NkAngle(70.f),1280.f/720.f,n,f);
 for(int c=0;c<4;c++)for(int r=0;r<4;r++)persp.mat[c][r]=mp.mat[c][r];
 print("symetrique",sym,persp);
 M4 quest=Projection(-52*rad,45*rad,48*rad,-50*rad,n,f);
 // Le second tableau est la comparaison avec les coefficients de NkCamera3D::SetFovAsym.
 // Cette matrice de référence explicite reprend les termes du contrat du moteur.
 NkCamera3D cam;cam.SetNearFar(n,f);cam.SetFovAsym(-52*rad,45*rad,48*rad,-50*rad);
 M4 ref{};const NkMat4f mc=cam.GetProj();for(int c=0;c<4;c++)for(int r=0;r<4;r++)ref.mat[c][r]=mc.mat[c][r];
 print("quest",quest,ref);
}
