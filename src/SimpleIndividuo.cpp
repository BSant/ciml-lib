#include "SimpleIndividuo.h"

SimpleIndividuo::SimpleIndividuo(bool inicializa)
{
    if(inicializa == true){
        for(int j = 0 ; j < conf->numTree; j++)
        {
            addTree(new Tree());
        }
    }else{
        //cout << "Inicializa == false " << endl;
        //cin.get();
    }
}
string SimpleIndividuo::nameIndividual(){
    return "SimpleIndividuo";
}

Subject* SimpleIndividuo::clone()
{
    //cout << "Clone " << endl;
    Subject* s = new SimpleIndividuo(false);

    for(int i = 0; i < conf->numTree; i++)
    {
        s->trees.push_back(trees.at(i)->clone());
    }
    s->fitness = fitness;

    return s;
}

SimpleIndividuo::~SimpleIndividuo()
{
    //cout << "Destrutor SimpleIndividuo" << endl;
   // cin.get();
    for(Tree* t : trees)
        delete t;
    trees.clear();
}
