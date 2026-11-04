from pathlib import Path

root = Path(__file__).parent
kit = root.parent / "chapitre-06" / "exo7-le_cout_mesure" / "cout.cpp"

def strip_cpp_comments(source):
    output = []
    i = 0
    state = "code"
    while i < len(source):
        ch = source[i]
        nxt = source[i + 1] if i + 1 < len(source) else ""
        if state == "code":
            if ch == '"':
                state = "string"
                output.append(ch)
            elif ch == "'":
                state = "char"
                output.append(ch)
            elif ch == "/" and nxt == "/":
                state = "line"
                i += 1
            elif ch == "/" and nxt == "*":
                state = "block"
                i += 1
            else:
                output.append(ch)
        elif state == "string" or state == "char":
            output.append(ch)
            if ch == "\\" and i + 1 < len(source):
                i += 1
                output.append(source[i])
            elif (state == "string" and ch == '"') or (state == "char" and ch == "'"):
                state = "code"
        elif state == "line":
            if ch == "\n":
                output.append(ch)
                state = "code"
        elif state == "block":
            if ch == "*" and nxt == "/":
                state = "code"
                i += 1
            elif ch == "\n":
                output.append(ch)
        i += 1
    return "".join(output)

base = strip_cpp_comments(kit.read_text(encoding="utf-8"))

exercises = {
1: ("exo1-votre_budget_calcule", "budget.cpp", "budget.txt", "reponse.txt"),
2: ("exo2-l_instant_predit_et_sa_periode", "instant.cpp", "instants.txt", "reponse.txt"),
3: ("exo3-mesurer_une_vitesse", "vitesse.cpp", "vitesses.txt", "reponse.txt"),
4: ("exo4-le_chemin_long_provoque", "chemin.cpp", "chemin.txt", "reponse.txt"),
5: ("exo5-predire_et_comparer", "prediction.cpp", "erreurs.txt", "reponse.txt"),
6: ("exo6-ce_que_la_prediction_vous_apporte", "gestes.cpp", "gestes.txt", "reponse.txt"),
7: ("exo7-la_latence_injectee_et_la_borne", "latence.cpp", "latences.txt", "courbe.png", "reponse.txt"),
8: ("exo8-la_stabilite_mesuree", "stabilite.cpp", "stabilite.txt", "reponse.txt"),
9: ("exo9-le_verrou_de_demi_cadence", "verrou.cpp", "verrou.txt", "reponse.txt"),
10: ("exo10-un_meilleur_modele", "modeles.cpp", "modeles.txt", "reponse.txt"),
}

labels = {
1: ("La fréquence de mon écran :", "La cadence que je tiens :", "Ce que dit le maximum :"),
2: ("Ce qui est stable :", "Un divisé par la cadence :", "La période de mon écran :"),
3: ("La série programmée :", "La série à la souris :", "D'où vient le bruit :"),
4: ("L'image fautive :", "Le rapport :", "Pourquoi à ce lacet :", "Ce qu'aurait vu le casque :"),
5: ("Le gain :", "D'où vient l'erreur qui reste :", "Le facteur dix :"),
6: ("Mes deux gestes :", "Où elle aide le moins :", "Pourquoi là :"),
7: ("Où elle cesse de rattraper :", "L'horizon sans borne :", "Ce que verrait le casque :"),
8: ("Le nombre qui compte :", "Pourquoi pas la moyenne :", "Ce que l'écran change :"),
9: ("Les deux écarts types :", "Ce que coûte le verrou :", "La décision du moteur :"),
10: ("Où il gagne :", "Où il perd :", "Pourquoi là :"),
}

notes = {
1: "budget 72 Hz : 13.889 ms\nbudget 90 Hz : 11.111 ms\nbudget 120 Hz : 8.333 ms\necran : à mesurer en lançant MASALLE_EXIT=1040\nsans attente : à mesurer avec NK_VK_MAILBOX=1",
2: "maintenant = à relever à l'exécution\nRépéter les vingt lignes à 72 Hz, puis relever reel - predit à 60, 90 et 120 Hz. Les périodes théoriques sont respectivement 16.667, 11.111 et 8.333 ms.",
3: "serie programme\n20 lignes image N : dt = ... ms, vitesse = (x, y, z) deg/s\n\nserie souris\n20 lignes image N : dt = ... ms, vitesse = (x, y, z) deg/s",
4: "sans le test\nimage avant : à relever\nimage fautive : à relever\nimage après : à relever\n\navec le test\nimage avant : à relever\nimage fautive : à relever\nimage après : à relever",
5: "avec prediction : moyenne ... deg, max ... deg, sur 1000 images\nsans prediction : moyenne ... deg, max ... deg, sur 1000 images",
6: "rapide (90 deg en 400 ms) : avec ... deg, sans ... deg, rapport ...\nlent : avec ... deg, sans ... deg, rapport ...\ndemi-tour : avec ... deg, sans ... deg, rapport ...",
7: "latence 0 ms : avec ... deg (max ...), sans ... deg (max ...)\nlatence 5 ms : avec ... deg (max ...), sans ... deg (max ...)\nlatence 10 ms : avec ... deg (max ...), sans ... deg (max ...)\nlatence 20 ms : avec ... deg (max ...), sans ... deg (max ...)\nlatence 40 ms : avec ... deg (max ...), sans ... deg (max ...)\nsans borne : échantillon ... s, horizon ... ms, prédit ... deg, vrai ... deg\navec borne : échantillon ... s, horizon ... ms, prédit ... deg, vrai ... deg",
8: "ecran : à mesurer pendant 60 s après 30 images de chauffe\nsans attente : à mesurer pendant 60 s avec NK_VK_MAILBOX=1",
9: "copies de la salle : à relever\nsans verrou : moyenne ... ms, ecart type ... ms, max ... ms\navec verrou : moyenne ... ms, ecart type ... ms, max ... ms",
10: "rapide (90 deg en 400 ms) : vitesse ... deg (max ...), acceleration ... deg (max ...)\nlent : vitesse ... deg (max ...), acceleration ... deg (max ...)\ndemi-tour : vitesse ... deg (max ...), acceleration ... deg (max ...)\narret : vitesse ... deg (max ...), acceleration ... deg (max ...)",
}

responses = {
1: "Les valeurs de fréquence et de cadence seront conclues depuis les deux séries mesurées; renseigner ici les mesures réelles avant dépôt.",
2: "Comparer les quatre écarts mesurés et les périodes 1/hz; compléter avec les valeurs affichées par le programme.",
3: "Comparer la stabilité de la série programmée et celle de la souris à partir des quarante valeurs relevées.",
4: "L'absence du choix du chemin court peut transformer un delta minuscule en rotation presque complète; compléter avec l'image et le rapport effectivement observés.",
5: "Le gain se calcule comme moyenne sans prédiction divisée par moyenne avec prédiction. Les valeurs dépendent des mouvements réellement enregistrés.",
6: "Comparer les erreurs moyennes des gestes calculées par le programme. Le modèle à accélération peut perdre près d'un arrêt ou d'un changement brusque.",
7: "Comparer les erreurs avec et sans borne aux retards mesurés; compléter avec l'horizon et les erreurs réellement produites.",
8: "Le maximum et le nombre d'images hors budget décrivent les retards visibles; compléter avec les deux mesures réelles.",
9: "Comparer les écarts types et les fréquences obtenues. Le verrou vise deux périodes d'écran; la décision se juge sur les mesures de cette machine.",
10: "Comparer moyenne et maximum sur les quatre gestes. Décrire les cas de perte avec les chiffres produits par le calcul.",
}


timing_include = '#include "NKTime/NkChrono.h"\n#include <algorithm>\n#include <cmath>\n#include <vector>\n'
timing_state = '''\tuint64 dernierDebutNs = 0;
\tstd::vector<double> intervallesMs;
\twhile (tourne && fenetre.IsOpen()) {
\t\tconst uint64 debutNs = NkChrono::Now().nanoseconds;
\t\tif (dernierDebutNs != 0 && image >= 30 && intervallesMs.size() < 1000) {
\t\t\tintervallesMs.push_back((double)(debutNs - dernierDebutNs) / 1.0e6);
\t\t}
\t\tdernierDebutNs = debutNs;
'''

for n, spec in exercises.items():
    folder = root / spec[0]
    folder.mkdir(parents=True, exist_ok=True)
    cpp = spec[1]
    if n in (1, 2, 8, 9):
        src = base.replace('#include <cstdlib>', '#include <cstdlib>\n' + timing_include)
        src = src.replace('\tNkWindowConfig cfg;', '\tif (' + str(n) + ' == 1) {\n\t\tstd::printf("budget 72 Hz : %.3f ms\\nbudget 90 Hz : %.3f ms\\nbudget 120 Hz : %.3f ms\\n", 1000.0/72.0, 1000.0/90.0, 1000.0/120.0);\n\t}\n\tNkWindowConfig cfg;', 1)
        src = src.replace('\twhile (tourne && fenetre.IsOpen()) {', timing_state, 1)

        src = src.replace('\tstd::printf("[MaSalle] %llu images\\n", (unsigned long long)image);', '''\tif (!intervallesMs.empty()) {
\t\tstd::sort(intervallesMs.begin(), intervallesMs.end());
\t\tdouble somme = 0.0, maxMs = 0.0, horsBudget = 0.0;
\t\tfor (double x : intervallesMs) { somme += x; maxMs = std::max(maxMs, x); if (x > 1000.0/90.0) horsBudget += 1.0; }
\t\tconst double moyenne = somme / intervallesMs.size();
\t\tconst double mediane = intervallesMs[intervallesMs.size()/2];
\t\tdouble variance = 0.0;
\t\tfor (double x : intervallesMs) variance += (x-moyenne)*(x-moyenne);
\t\tvariance /= intervallesMs.size();
\t\tstd::printf("mesure : N=%zu, moyenne=%.3f ms, mediane=%.3f ms, ecart type=%.3f ms, max=%.3f ms, hors budget=%llu\\n", intervallesMs.size(), moyenne, mediane, std::sqrt(variance), maxMs, (unsigned long long)horsBudget);
\t}
\tstd::printf("[MaSalle] %llu images\\n", (unsigned long long)image);''', 1)
        if n == 2:
            src = src.replace('uint64 dernierDebutNs = 0;', 'uint64 dernierDebutNs = 0;\n\tconst char *hzEnv = std::getenv("MASALLE_HZ");\n\tconst double hz = hzEnv ? std::atof(hzEnv) : 72.0;\n\tconst double periodeNs = 1.0e9 / hz;\n\tconst uint64 origineNs = NkChrono::Now().nanoseconds;\n\tdouble precedentPredit = 0.0;\n\tunsigned lignesPredit = 0;')
            src = src.replace('\t\tdernierDebutNs = debutNs;', '''\t\tdernierDebutNs = debutNs;
\t\tconst double maintenant = (double)(debutNs - origineNs);
\t\tconst double predit = maintenant + periodeNs;
\t\tif (lignesPredit < 20) { std::printf("maintenant = %.3f ms, predit = %.3f ms, ecart = %.3f ms\\n", maintenant/1.0e6, predit/1.0e6, (lignesPredit ? ((double)debutNs-precedentPredit)/1.0e6 : 0.0)); ++lignesPredit; }
\t\tprecedentPredit = predit;''', 1)
        if n == 9:
            src = src.replace('uint64 dernierDebutNs = 0;', 'uint64 dernierDebutNs = 0;\n\tconst double periodeEcranMs = 1000.0 / 60.0;')
            src = src.replace('\t\tdernierDebutNs = debutNs;', '''\t\tdernierDebutNs = debutNs;
\t\tif (EnvU64("MASALLE_VERROU", 0) != 0) {
\t\t\tconst double elapsedMs = (double)(NkChrono::Now().nanoseconds - debutNs) / 1.0e6;
\t\t\tif (elapsedMs < 2.0 * periodeEcranMs) NkChrono::SleepMilliseconds((uint64)(2.0 * periodeEcranMs - elapsedMs));
\t\t}''', 1)
    else:
        src = base

    if n in (3, 4, 5):
        src = src.replace('#include "NKRenderer/Tools/Render3D/NkRender3D.h"', '#include "NKRenderer/Tools/Render3D/NkRender3D.h"\n#include "NKMath/NkQuat.h"\n#include "NKTime/NkChrono.h"\n#include "NKEvent/NkMouseEvent.h"')
        angular = '''
NkVec3f VitesseAngulaire(const NkQuatf &from, const NkQuatf &to, float32 dtSeconds) {
    if (dtSeconds <= 0.f) return NkVec3f(0.f, 0.f, 0.f);
    NkQuatf delta = (to * from.Conjugate()).Normalized();
    if (delta.w < 0.f) delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);
    const float32 angleRad = delta.Angle().Rad();
    if (angleRad <= 1e-6f) return NkVec3f(0.f, 0.f, 0.f);
    return delta.Axis() * (angleRad / dtSeconds);
}
NkQuatf Extrapoler(const NkQuatf &orientation, const NkVec3f &vitesse, float32 dtSeconds) {
    const float32 speedRad = vitesse.Len();
    if (speedRad <= 1e-6f) return orientation;
    const NkVec3f axis = vitesse * (1.f / speedRad);
    const NkQuatf delta(NkAngle::FromRad(speedRad * dtSeconds), axis);
    return (delta * orientation).Normalized();
}
'''
        if n == 4:
            angular = angular.replace('    if (delta.w < 0.f) delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);\n', '')
        src = src.replace('uint64 EnvU64(', angular + '\n\tuint64 EnvU64(', 1)
        src = src.replace('NkEvent *ev = nullptr;\n\t\twhile (NkEvents().PollEvent(ev)) {', '''NkEvent *ev = nullptr;
\t\tfloat32 deltaX = 0.f, deltaY = 0.f;
\t\twhile (NkEvents().PollEvent(ev)) {
\t\t\tif (auto *raw = ev->As<NkMouseRawEvent>()) { deltaX += raw->GetDeltaX(); deltaY += raw->GetDeltaY(); }''', 1)
        src = src.replace('uint64 image = 0;', '''float32 lacetRad = 0.f, tangageRad = 0.f;
\tNkQuatf orientationPrecedente = NkQuatf::Identity();
\tuint64 instantPrecedent = 0;
\tuint64 image = 0;''', 1)
        src = src.replace('if (!tourne) break;\n\n\t\tif (rEcran->BeginFrame()) {', '''if (!tourne) break;
\t\tlacetRad -= deltaX * 0.0022f;
\t\ttangageRad -= deltaY * 0.0022f;
\t\tconst NkMat4f rotationTete = NkMat4f::RotationY(NkAngle::FromRad(lacetRad)) * NkMat4f::RotationX(NkAngle::FromRad(tangageRad));
\t\tconst NkQuatf orientation = NkQuatf(rotationTete).Normalized();
\t\tconst uint64 instant = NkChrono::Now().nanoseconds;
\t\tif (instantPrecedent != 0) { const float32 dt = (float32)(instant - instantPrecedent) / 1.0e9f; const NkVec3f vitesse = VitesseAngulaire(orientationPrecedente, orientation, dt); if (image < 20) std::printf("image %llu : dt = %.3f ms, vitesse = (%.3f, %.3f, %.3f) deg/s\\n", (unsigned long long)image, (double)(dt*1000.f), (double)(vitesse.x*57.29578f), (double)(vitesse.y*57.29578f), (double)(vitesse.z*57.29578f)); }
\t\torientationPrecedente = orientation; instantPrecedent = instant;

\t\tif (rEcran->BeginFrame()) {''', 1)
        src = src.replace('cd.target = { -0.70f, 1.10f, -kDemi };', 'cd.target = cd.position + orientation * NkVec3f(0.f, 0.f, -1.f);', 1)
    if not (folder / cpp).exists():
        (folder / cpp).write_text(strip_cpp_comments(src), encoding="utf-8")
    for item in spec[2:]:
        if item == "reponse.txt":
            content = "\n\n".join(label + " " + responses[n] for label in labels[n]) + "\n"
        elif item == "courbe.png":
            continue
        else:
            content = notes[n] + "\n"
        if not (folder / item).exists():
            (folder / item).write_text(content, encoding="utf-8")


offline = r'''#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
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
'''
offline_helpers = offline[:offline.index('int main() {')]
for n, target in ((6, "gestes.cpp"), (10, "modeles.cpp")):
    src = base.replace('#include <cstdlib>', '#include <cstdlib>\n#include <algorithm>\n#include <cmath>\n#include <string>\n#include <vector>')
    src = src.replace('using namespace nkentseu;', offline_helpers + '\nusing namespace nkentseu;', 1)
    output = '''\tstd::puts("rapide (90 deg en 400 ms):"); evaluate("rapide", .4); std::puts("");
\tstd::puts("lent:"); evaluate("lent", 1.2); std::puts("");
\tstd::puts("demi-tour:"); evaluate("demi-tour", .25); std::puts("");'''
    if n == 10:
        output += '\n\tstd::puts("arret:"); evaluate("arret", .6); std::puts("");'
    src = src.replace('\tNkWindowConfig cfg;', output + '\n\tNkWindowConfig cfg;', 1)
    if not (root / exercises[n][0] / target).exists():
        (root / exercises[n][0] / target).write_text(src, encoding="utf-8")
