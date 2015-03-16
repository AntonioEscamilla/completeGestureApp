#include "analizador.h"

Analizador::Analizador ( ) {    
    actPos = 0;
    oldPos = 0;
    oldV = 0;
    actV = 0;
    oldPend = " ";
    actPend = " ";
    hit = " ";
}

float Analizador::calcVel() {
    actV = actPos - oldPos;
}

std::string Analizador::calcPend(){
    if (actV > 0.5) {
        actPend = "positiva";
    } else if (actV < -1) {
        actPend = "negativa";
    }else{
        actPend = "cero";
    }
    return actPend;
}


std::string Analizador::maximo(){
    if (actPend=="negativa" && oldPend=="positiva"){
        hit="hit";
        oldPend=actPend;
        return hit;
    }
    hit=" ";
    oldPend=actPend;
    return hit;
}

std::string  Analizador::calcularTodo(float posAct){
    actPos=posAct;
    
    calcVel();
    oldPos=actPos;
    
    calcPend();
    oldV=actV;
    
    return maximo();
}
