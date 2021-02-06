#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

using namespace std;


class State
{
public:
    string Name;
    bool First;
    bool Final;
    map<string,string> AvailableTransitions;

    State(string,bool,bool,vector<string>);
    State(string,bool,bool,map<string,string>);
    State(vector<string>);
    State operator+(State);
};

class NFAutomaton
{
public:
    vector<State> States;
    int FirstState;
    string FinalState;
    vector<string> Alphabet;


    NFAutomaton(string);
};

class DFAutomaton
{
public:
    vector<State> States;
    int FirstState;
    string FinalState;
    vector<string> Alphabet;

    DFAutomaton(NFAutomaton);
};

State::State(vector<string> Alphabet)
{
    Name = "";
    First = false;
    Final = false;

    for(int i = 0; i < Alphabet.size(); i ++)
    {
        AvailableTransitions[Alphabet[i]] = "";
    }
}

State::State(string n,bool First,bool Final, vector<string> Alphabet)
{
    Name = n;
    this -> First = First;
    this -> Final = Final;

    for(int i = 0; i < Alphabet.size(); i ++)
    {
        AvailableTransitions[Alphabet[i]] = "";
    }
}

State::State(string n,bool First,bool Final,map<string,string> Transitions)
{
    Name = n;
    this -> First = First;
    this -> Final = Final;
    AvailableTransitions = Transitions;
}

State State::operator+( State s)
{
    string NewName = Name;
    bool NewFirst,NewFinal;
    map<string,string> NewAvailableTransitions = AvailableTransitions;
    
    
    string substring;
    //+2 as I have q and number
    for(int i = 0; i < s.Name.length(); i = i+2)
    {

        substring = s.Name.substr(i,2);
        bool flag = false;

        for(int z = 0; z < NewName.length(); z++)
        {
            if ( NewName.substr(z,2) > substring )
            {
                NewName.insert( z, substring );
                flag = true;
                break;
            }
            else if ( NewName.substr(z,2) == substring )
            {
                flag = true;
                break;
            }
            
        }
        if(!flag)
        {
            NewName += substring;
        }
    }
    
    NewFirst = false;

    if ( ( this -> Final == true ) || ( s.Final == true ) )
    {
        NewFinal = true;
    }
    else
    {
        NewFinal = false;
    }



    //Creation of New map BE CAREFUL
    //Same Symbols. For each symbol, Integrate the strings char to char like NFA Constructor
    //Suggestion, Check NFA Constructor first.
    string Symbol;
    map<string,string>::iterator it;
    for(it = AvailableTransitions.begin(); it != AvailableTransitions.end() ; it++)
    {
        Symbol = it -> first;
        string tempString1 = s.AvailableTransitions[Symbol];

        for(int i = 0; i < tempString1.length() ; i++)
        {

            bool flag = false;

            for(int z = 0; z < NewAvailableTransitions[Symbol].length(); z++)
            {
                //String[i] is a char so it needs to be converted to string so we can use the operator '>'
                if( string(1,NewAvailableTransitions[Symbol][z]) > string(1,tempString1[i]) )
                {
                    NewAvailableTransitions[Symbol].insert( z, string(1,tempString1[i]) );
                    flag = true;
                    break;
                }
                else if ( string(1,NewAvailableTransitions[Symbol][z]) == string(1,tempString1[i]) )
                {
                    flag = true;
                    break;
                }
                
            }
            if(!flag)
            {
                NewAvailableTransitions[Symbol] += tempString1[i];
            }

        }
        

    }
    

    return State(NewName,NewFirst,NewFinal,NewAvailableTransitions);

}

NFAutomaton::NFAutomaton(string Filename)
{
    ifstream File (Filename);
    string line;
    int NumberOfStates;
    
   


    //Read Number of States
    File >> NumberOfStates;
    

    //Read and Parse the Alphabet
    getline(File,line); // getting rid of empty line remnant
    getline(File,line); // reading line of alphabet
    size_t pos = 0; //initialization for substr 
    string token,delimiter = " "; //defining the delimeter
    while ((pos = line.find(delimiter)) != delimiter.npos) //repeatitive parsing for alphabet length > 2 
    {
        token = line.substr(0, pos); //get substr
        Alphabet.insert(Alphabet.end(), token);  //store it
        line.erase(0, pos + delimiter.length()); //erasure necessary due to string.find() used 
    }
    Alphabet.insert(Alphabet.end(),line);


    //Read the First State
    File >> FirstState;
    

    getline(File,line);
    //Read the Final State
    getline(File,FinalState);


    //Create the States
    for(int i = 0; i < NumberOfStates; i++)
    {
        bool first = false;
        bool final = false;

        //So that we can dynamically name them, turn i into a char and add it at the end of q
        //i.e. if i = 0, then the name of the state will be q0. i = 2 , q2 so on so forth
        char c = '0' + i;
        string name = "q";
        name += c;

        if (i == FirstState)
        {
            first = true;
        }

        if (FinalState.find(to_string(i)) !=  FinalState.npos )
        {
            final = true;
        }

        States.insert(States.end(), State(name,first,final,Alphabet));
        
    }



    // Add the Tranfer Functions at the appropriate States
    int StateSource;
    string Symbol, StateDestination;
    int count = 0;

    while(!File.eof())
    {
        //Read the info into variables
        File >> StateSource;
        File >> Symbol >> StateDestination;



        //Insert new destination in the correct position
        bool flag = false;

        for(int i = 0; i < States[StateSource].AvailableTransitions[Symbol].length(); i++)
        {
            //String[i] is a char so it needs to be converted to string so we can use the operator '>'
            if( string(1,States[StateSource].AvailableTransitions[Symbol][i]) > StateDestination)
            {
                States[StateSource].AvailableTransitions[Symbol].insert(i,StateDestination);
                flag = true;
                break;
            }
            else if (string(1,States[StateSource].AvailableTransitions[Symbol][i]) == StateDestination)
            {
                flag = true;
                break;
            }
            
        }
        if(!flag)
        {
            States[StateSource].AvailableTransitions[Symbol] += StateDestination;
        }

    }

    File.close();
}

DFAutomaton::DFAutomaton(NFAutomaton nfa)
{
    Alphabet = nfa.Alphabet;
    FirstState = nfa.FirstState;
    States.insert(States.end(), nfa.States[FirstState]);
    
    
    int counter = 0;
    for(counter; counter < States.size(); counter++)
    {
    
        for(int i = 0; i < Alphabet.size(); i++)
        {
            State temp(Alphabet);
            int pointer;
            for(int z = 0; z < States[counter].AvailableTransitions[ Alphabet[i] ].size(); z++ )
            {  
                pointer = States[counter].AvailableTransitions[ Alphabet[i] ][z] - '0';
                temp = nfa.States[pointer] + temp;
            }

            bool Exists = false;
            for(int z = 0; z < States.size(); z++)
            {
                if(States[z].Name == temp.Name)
                {
                    Exists = true;
                }
            }
            if (!Exists)
            {
                States.insert(States.end(), temp);
                if(temp.Final)
                {
                    FinalState += " ";
                    FinalState += temp.Name;
                }
            }
        }

        
    }


}

int main(void)
{

    NFAutomaton a("Dfa.txt");


    
    DFAutomaton d(a);
    


    vector<State>::iterator it = d.States.begin();
    for(it; it != d.States.end(); it++)
    {
        
        cout << it->Name << " " << it->AvailableTransitions["0"] << " " <<  it->AvailableTransitions["1"] << endl;
        
    }

    return 0;
}