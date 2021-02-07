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
    friend ostream& operator<<(ostream& os, DFAutomaton& dfa);
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
                    FinalState += temp.Name;
                    FinalState += " ";
                }
            }
        }   
    }
}

ostream& operator<<(ostream& os, DFAutomaton& dfa)
{
    //Print number of states
    os << dfa.States.size() << "\t// To αυτόματο έχει " << dfa.States.size() << " καταστάσεις" << endl;
    
    //Print alphabet
    for(int i=0; i<dfa.Alphabet.size(); i++)
    {
        os << dfa.Alphabet[i] << " ";
    }
    os << "\t// To αλφάβητο έχει " << dfa.Alphabet.size() << " σύμβολα" << endl;

    //Print First State
    os << dfa.FirstState << "\t// Αρχική κατάσταση είναι η q" << dfa.FirstState << endl;

    //Print Final State
    //Format it as needed (remove q)
    string s = dfa.FinalState;
    size_t pos = 0; //initialization for substr 
    string token,StrippedQString = "",delimiter = "q"; //defining the delimeter
    while ((pos = s.find(delimiter)) != delimiter.npos) //repeatitive parsing for alphabet length > 2 
    {
        token = s.substr(0, pos); //get substr
        StrippedQString += token ;//store it
        s.erase(0, pos + delimiter.length()); //erasure necessary due to string.find() used 
    }
    StrippedQString += s ;
    
    os << StrippedQString << "\t// Τελική/ές κατάσταση/εις είναι η/οι " << dfa.FinalState << endl;

    //Print State Transitions
    for(int i = 0; i < dfa.States.size(); i++)
    {

        if(dfa.States[i].Name == "")
        {
            dfa.States[i].Name = "ε";
        }

        string OriginState = dfa.States[i].Name;
        size_t pos = 0; //initialization for substr 
        string delimiter = "q"; //defining the delimeter
        while ((pos = OriginState.find(delimiter)) != delimiter.npos) //repeatitive parsing for alphabet length > 2 
        {
            OriginState.replace(pos,1,"");
        }

        for(int j = 0; j < dfa.Alphabet.size(); j++)
        {
            string DestinationState = dfa.States[i].AvailableTransitions[dfa.Alphabet[j]];
            
            if(DestinationState == "")
            {
                DestinationState = "ε";
            }

            os << OriginState << " " << dfa.Alphabet[j] << " " << DestinationState;
            os << "\t// Αν το αυτόματο είναι στην κατάσταση " << dfa.States[i].Name;
            os << " με " << dfa.Alphabet[j] << " θα μεταβεί στην κατάσταση ";
            
            string DestinationStateWithQ = "";

            if (DestinationState != "ε")
            {
                for(int z = 0; z < DestinationState.length(); z++)
                {
                    DestinationStateWithQ += "q";
                    DestinationStateWithQ += DestinationState[z]; 
                }
            }
            else
            {
                DestinationStateWithQ = "ε"; 
            }
            
            
            os << DestinationStateWithQ << endl;
        }
    }


    return os;
}

int main(void)
{

    NFAutomaton a("Dfa.txt");

    DFAutomaton d(a);

    ofstream file("DeterministocFA.txt");
    file << d;

    return 0;
}