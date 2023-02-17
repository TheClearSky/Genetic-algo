#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <utility>
#include <numeric>

using std::cout;
using std::cin;
using std::vector;
using std::string;
using std::max;
using std::pair;
using std::sort;

void initializerandom() //initializes getrandom with time seed
{
    std::srand(std::time(0));
}
int getrandom(const int min,const int max)//including min and max
{
    return (rand()%(max-min+1))+min;
}
void printrepeatingchars(const char c,int times) //prints char c, "times" times
{
    if(times<0)return;
    while(times--)
    {
        cout<<c;
    }
}
int digitsinint(int num) // how many digits are in num
{
    if(num==0) return 1;
    int ans=0;
    while(num)
    {
        ++ans;
        num/=10;
    }
    return ans;
}
template<class T> void shufflevector(vector<T> &vec) //shuffles a vector using default random number generator and a time based seed
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::shuffle (vec.begin(), vec.end(), std::default_random_engine(seed));
}

//prints the message in a box in the below format
//+-------+
//|message|
//+-------+
void printinbox(const string message,int spacefromleft=0)
{
    //line 1
    printrepeatingchars(' ',spacefromleft);
    cout<<'+';
    printrepeatingchars('-',message.length());
    cout<<"+\n";

    //line 2
    printrepeatingchars(' ',spacefromleft);
    cout<<'|'<<message<<"|\n";
    
    //line 3
    printrepeatingchars(' ',spacefromleft);
    cout<<'+';
    printrepeatingchars('-',message.length());
    cout<<"+\n";
}

//a valid solution of the problem
class Solution{
    
    int id; //solution id
    static int count; //total number of solutions generated till now
    int numberoftasks; 
    int numberofprocessors;
    vector<int> chromosome;

    //following variables are needed for printing 
    vector<int> maxcellwidth; //max width of ith cell in the chromosome table
    string label1="Tasks:";  //name of row 1 in chromosome table
    string label2="Processors:"; //name of row 2 in chromosome table
    int leftspacing=1; //left spacing of every cell in chromosome table
    int rightspacing=1; //right spacing of every cell in chromosome table

    public:
    
    //initializes the variables needed to build the chromosome and spacing in table
    void init(const int &totaltasks,const int &totalprocessors,const int &leftspace=1,const int &rightspace=1)
    {
        id=count++;
        leftspacing=leftspace;
        rightspacing=rightspace;
        numberoftasks=totaltasks;
        numberofprocessors=totalprocessors;
    }

    //constructs a solution and initializes the chromosome with random values between all processors
    Solution(const int &totaltasks,const int &totalprocessors,const int &leftspace=1,const int &rightspace=1)
    {
        init(totaltasks,totalprocessors,leftspace,rightspace);
        chromosome.resize(numberoftasks);
        maxcellwidth.resize(numberoftasks);
        int i=1;
        for(auto &gene:chromosome)
        {
            gene=getrandom(1,numberofprocessors); //every gene gets a random value in range [1,numberofprocessors]
            maxcellwidth[i-1]=digitsinint(max(i,gene)); //for printing
            ++i;
        }
    }

    Solution(const int &totaltasks,const int &totalprocessors,vector<int> &inchromosome,const int &leftspace=1,const int &rightspace=1)
    {
        init(totaltasks,totalprocessors,leftspace,rightspace);
        chromosome=std::move(inchromosome);
        maxcellwidth.resize(numberoftasks);
        int i=1;
        for(auto &gene:chromosome)
        {
            maxcellwidth[i-1]=digitsinint(max(i,gene));
            ++i;
        }
    }
    void printchromosome()
    {
        printinbox("Solution "+std::to_string(id));
        //line 1
        printrepeatingchars('-',max(label1.length(),label2.length()));
        cout<<'+';
        for(auto &width:maxcellwidth)
        {
            printrepeatingchars('-',leftspacing+width+rightspacing);
            cout<<'+';
        }
        //line 2
        cout<<'\n'<<label1;
        printrepeatingchars(' ',label2.length()-label1.length());
        cout<<'|';
        for(int i=1;i<=numberoftasks;++i)
        {
            printrepeatingchars(' ',leftspacing);
            cout<<i;
            printrepeatingchars(' ',maxcellwidth[i-1]-digitsinint(i));
            printrepeatingchars(' ',rightspacing);
            cout<<'|';
        }
        //line 3
        cout<<'\n'<<label2;
        printrepeatingchars(' ',label1.length()-label2.length());
        cout<<'|';
        for(int i=1;i<=numberoftasks;++i)
        {
            printrepeatingchars(' ',leftspacing);
            cout<<chromosome[i-1];
            printrepeatingchars(' ',maxcellwidth[i-1]-digitsinint(chromosome[i-1]));
            printrepeatingchars(' ',rightspacing);
            cout<<'|';
        }
        cout<<'\n';
        //line 4
        printrepeatingchars('-',max(label1.length(),label2.length()));
        cout<<'+';
        for(auto &width:maxcellwidth)
        {
            printrepeatingchars('-',leftspacing+width+rightspacing);
            cout<<'+';
        }
        cout<<'\n';
    }
    int &getandsetprocessorassigned(const int taskindex)
    {
        return chromosome[taskindex-1];
    }
    int getid()
    {
        return id;
    }
    Solution crossover(Solution &sol)
    {
        int crossoverindex=getrandom(1,numberoftasks-1);
        vector<int> childchromosome(numberoftasks,0);
        for(int i=0;i<crossoverindex;++i)
        {
            childchromosome[i]=chromosome[i];
        }
        for(int i=crossoverindex;i<numberoftasks;++i)
        {
            childchromosome[i]=sol.getandsetprocessorassigned(i+1);
        }
        return Solution(numberoftasks,numberofprocessors,childchromosome,leftspacing,rightspacing);
    }
};
int Solution::count=1;
class Evaluator{
    vector<vector<int>> processortimematrix;
    int numberoftasks;
    int numberofprocessors;

    vector<int> maxcellwidth; //for matrix UI
    string indexlabel="Tasks:";
    string restlabel="Processor#";
    int leftspacing=1;
    int rightspacing=1;
    int maxsizeoflabels;
    
    vector<vector<int>> lastevaluation;//for previous evaluation
    vector<int> releasetimes;
    int lastevaluationreleasetime=-1;

    vector<int> previousevalmaxcellwidth;
    string allocationlabel="T";//for previous evaluation UI
    vector<int> prevevalpostfixwidthsum;

    public:
    void updatematrix(int minprocesstime,int maxprocesstime)
    {
        for(int i=1;i<=numberoftasks;++i)
        {
            maxcellwidth[i-1]=digitsinint(i);
        }
        for(auto &processor:processortimematrix)
        {
            int j=1;
            for(auto &tasktimeinthisprocessor:processor)
            {
                tasktimeinthisprocessor=getrandom(minprocesstime,maxprocesstime);
                maxcellwidth[j-1]=max(maxcellwidth[j-1],digitsinint(tasktimeinthisprocessor));
                ++j;
            }
        }
    }
    Evaluator(int totaltasks,int totalprocessors,int minprocesstime,int maxprocesstime,int leftspace=1,int rightspace=1)
    {
        numberoftasks=totaltasks;
        numberofprocessors=totalprocessors;
        leftspacing=leftspace;
        rightspacing=rightspace;
        processortimematrix.resize(numberofprocessors,vector<int>(numberoftasks));
        maxcellwidth.resize(numberoftasks,0);
        updatematrix(minprocesstime,maxprocesstime);
        int maxsizeof2ndlabel=restlabel.length()+digitsinint(numberofprocessors)+1;
        maxsizeoflabels=max(static_cast<int>(indexlabel.length()),maxsizeof2ndlabel);
    }
    int evaluate(Solution &sol)
    {
        releasetimes.clear();
        releasetimes.resize(numberofprocessors);
        lastevaluation.clear();
        lastevaluation.resize(numberofprocessors);
        previousevalmaxcellwidth.clear();
        for(int i=1;i<=numberoftasks;++i)
        {
            auto &processor=sol.getandsetprocessorassigned(i);
            releasetimes[processor-1]+=processortimematrix[processor-1][i-1];
            lastevaluation[processor-1].push_back(i);

            int lastinsertindex=lastevaluation[processor-1].size()-1;
            if(lastinsertindex>=previousevalmaxcellwidth.size())
            {
                previousevalmaxcellwidth.push_back(0);
            }
            previousevalmaxcellwidth[lastinsertindex]=max(previousevalmaxcellwidth[lastinsertindex],digitsinint(i)+digitsinint(processortimematrix[processor-1][i-1]));
        }

        int numberofcells=previousevalmaxcellwidth.size();
        prevevalpostfixwidthsum.clear();
        prevevalpostfixwidthsum.resize(numberofcells+1,0);
        for(int i=numberofcells-1;i>=0;--i)
        {
            prevevalpostfixwidthsum[i]=prevevalpostfixwidthsum[i+1]+previousevalmaxcellwidth[i];
        }

        int maxtime=0;
        for(auto &time:releasetimes)
        {
            maxtime=max(maxtime,time);
        }
        
        lastevaluationreleasetime=maxtime;
        return maxtime;
    }
    void mutatesolution(Solution &sol)
    {
        evaluate(sol);
        int currmin=releasetimes[0];
        int minindex=0;
        int currmax=currmin;
        int maxindex=0;
        for(int i=0;i<numberofprocessors;++i)
        {
            if(releasetimes[i]>currmax)
            {
                currmax=releasetimes[i];
                maxindex=i;
            }
            if(releasetimes[i]<currmin)
            {
                currmin=releasetimes[i];
                minindex=i;
            }
        }
        int maxtasknumber=lastevaluation[maxindex][lastevaluation[maxindex].size()-1];
        sol.getandsetprocessorassigned(maxtasknumber)=minindex+1;
        
    }
    void printlastevaluation()
    {
        printrepeatingchars('-',maxsizeoflabels);
        cout<<'+';
        for(auto &width:previousevalmaxcellwidth)
        {
            printrepeatingchars('-',leftspacing+rightspacing+width+allocationlabel.size()+2);
            cout<<'+';
        }
        cout<<'\n';

        for(int i=1;i<=numberofprocessors;++i)
        {
            int processortotal=0;
            cout<<restlabel<<i<<':';
            printrepeatingchars(' ',maxsizeoflabels-(restlabel.length()+digitsinint(i)+2));
            cout<<'|';
            int j=0;
            for(auto &task:lastevaluation[i-1])
            {
                auto &timeforthistask=processortimematrix[i-1][task-1];
                processortotal+=timeforthistask;
                printrepeatingchars(' ',leftspacing);
                cout<<allocationlabel<<task<<'('<<timeforthistask<<')';
                int currentwidth=digitsinint(task)+digitsinint(timeforthistask);
                printrepeatingchars(' ',previousevalmaxcellwidth[j]-currentwidth);
                printrepeatingchars(' ',rightspacing);
                cout<<'|';
                ++j;
            }
            int restspacing=prevevalpostfixwidthsum[lastevaluation[i-1].size()];
            restspacing+=(previousevalmaxcellwidth.size()-lastevaluation[i-1].size())*(leftspacing+allocationlabel.size()+2+rightspacing+1);
            printrepeatingchars(' ',restspacing);
            cout<<"Total:"<<processortotal<<'\n';
        }

        printrepeatingchars('-',maxsizeoflabels);
        cout<<'+';
        for(auto &width:previousevalmaxcellwidth)
        {
            printrepeatingchars('-',leftspacing+rightspacing+width+allocationlabel.size()+2);
            cout<<'+';
        }
        cout<<"Max Release Time:"<<lastevaluationreleasetime;
        cout<<'\n';
    }
    void printprocesstimes()
    {
        //line 1
        // int maxsizeof2ndlabel=restlabel.length()+digitsinint(numberofprocessors)+2;
        // int maxsizeoflabels=max(static_cast<int>(indexlabel.length()),maxsizeof2ndlabel);
        printrepeatingchars('-',maxsizeoflabels);
        cout<<'+';
        for(auto &width:maxcellwidth)
        {
            printrepeatingchars('-',leftspacing+width+rightspacing);
            cout<<'+';
        }
        //line 2
        cout<<'\n'<<indexlabel;
        printrepeatingchars(' ',maxsizeoflabels-indexlabel.length());
        cout<<'|';
        for(int i=1;i<=numberoftasks;++i)
        {
            printrepeatingchars(' ',leftspacing);
            cout<<i;
            printrepeatingchars(' ',maxcellwidth[i-1]-digitsinint(i));
            printrepeatingchars(' ',rightspacing);
            cout<<'|';
        }
        cout<<'\n';
        //rest lines
        int i=1;
        for(auto &processor:processortimematrix)
        {
            cout<<restlabel<<i<<':';
            printrepeatingchars(' ',maxsizeoflabels-(restlabel.length()+digitsinint(i)+2));
            cout<<'|';
            int j=1;
            for(auto &time:processor)
            {
                printrepeatingchars(' ',leftspacing);
                cout<<time;
                printrepeatingchars(' ',maxcellwidth[j-1]-digitsinint(time));
                printrepeatingchars(' ',rightspacing);
                cout<<'|';
                ++j;
            }
            cout<<'\n';
            ++i;
        }
        //line 4
        printrepeatingchars('-',maxsizeoflabels);
        cout<<'+';
        for(auto &width:maxcellwidth)
        {
            printrepeatingchars('-',leftspacing+width+rightspacing);
            cout<<'+';
        }
        cout<<'\n';
    }
};

class Menu{
    public:
    vector<string> options;
    string menuname;

    Menu(const vector<string> &inoptions,const string inmenuname="Menu"):options{inoptions},menuname{inmenuname}
    {}

    int startmenuandgetoption()
    {
        int currentselection=0;
        return startmenuandgetoption(currentselection);
    }

    //returns option number starting from 0 and -1 if exit
    int startmenuandgetoption(int &currentselection)
    {
        string input="";
        while(true)
        {
            if(std::cin.fail())
            {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            }
            printrepeatingchars('-',50);
            cout<<"\n\n";
            printinbox(menuname,10);
            cout<<"\n\n";
            for(int i=0;i<options.size();++i)
            {
                if(currentselection==i)
                {
                    printinbox(std::to_string(i+1)+". "+options[i]);
                }
                else
                {
                    cout<<" "<<i+1<<". "<<options[i]<<"\n";
                    if((i+1)!=(currentselection))
                    {
                        cout<<'\n';
                    }
                }
            }
            cout<<"Press u and enter to move selection up\n";
            cout<<"Press d and enter to move selection down\n";
            cout<<"Type an option number and enter to select that option\n";
            cout<<"Type ok to confirm current selection\n";
            cout<<"Type exit and enter to exit\n";
            bool isoutputvalid;
            do
            {
                isoutputvalid=true;
                cin>>input;
                transform(input.begin(), input.end(), input.begin(), ::tolower);
                if(input=="exit")
                {
                    return -1;
                }
                if(input=="u")
                {
                    if(currentselection>0)
                    {
                        --currentselection;
                    }
                }
                else if(input=="d")
                {
                    if(currentselection<options.size()-1)
                    {
                        ++currentselection;
                    }
                }
                else if(input=="ok")
                {
                    return currentselection;
                }
                else
                {
                    try
                    {
                        int optionnumber=std::stoi(input);
                        --optionnumber;
                        if((optionnumber<0)||(optionnumber>=options.size()))
                        {
                            cout<<"Invalid option number,Try again\n";
                            isoutputvalid=false;
                        }
                        else
                        {
                            currentselection=optionnumber;
                        }
                    }
                    catch(const std::exception& e)
                    {
                        cout<<"Invalid input,Try again\n";
                        isoutputvalid=false;
                    }
                }
            }while(!isoutputvalid);
            printrepeatingchars('\n',3);
        }
        return -1;
    }
};

enum class SelectionMode{
    rank,
    tournament,
    roulettewheel
};

//settings for the running of Genetic algo
class Settings{
    public:
    int numberoftasks=120;
    int numberofprocessors=20;
    int minprocessingtime=10;
    int maxprocessingtime=1000;

    int maxpopulationsize=100;
    int selectionsize=20;
    int tournamentsize=5;
    int roulettesize=5;
    int numberofgenerations=20;

    //for selecting the algorithm of selection step
    SelectionMode mode{SelectionMode::rank};

    //UI Settings
    bool notifyinitialization=true;
    bool printexecutionmatrix=false;
    bool printcurrentgeneration=false;
    bool printcurrentevaluation=false;
    bool printselection=true;
    bool printcrossoverandmutation=false;
    int samplesizetoprint=5;

    void turnoffallprinting()
    {
        notifyinitialization=false;
        printexecutionmatrix=false;
        printcurrentgeneration=false;
        printcurrentevaluation=false;
        printselection=false;
        printcrossoverandmutation=false;
    }
    void turnonallprinting()
    {
        notifyinitialization=true;
        printexecutionmatrix=true;
        printcurrentgeneration=true;
        printcurrentevaluation=true;
        printselection=true;
        printcrossoverandmutation=true;
    }
    string getcurrentselectionmode()
    {
        switch (mode)
        {
        case SelectionMode::rank:
            return "Rank";
        case SelectionMode::tournament:
            return "Tournament";
        case SelectionMode::roulettewheel:
            return "Roulette Wheel";
        
        default:
            break;
        }
        return "Invalid Selection Mode";
    }
};
class AlgorithmManager{
    vector<pair<Solution,int>> populationandfitness;
    Settings settings;
    Evaluator eval;
    static int generationnumber;
    public:
    AlgorithmManager():settings(),eval(settings.numberoftasks,settings.numberofprocessors,settings.minprocessingtime,settings.maxprocessingtime)
    {
    }
    AlgorithmManager(Settings &customsettings):settings{customsettings},eval(settings.numberoftasks,settings.numberofprocessors,settings.minprocessingtime,settings.maxprocessingtime)
    {
    }
    void printexecutionmatrix()
    {
        printinbox("Execution Time matrix");
        eval.printprocesstimes();
    }
    void generateinitialpopulation()
    {
        if(settings.printexecutionmatrix)
        {
            printexecutionmatrix();
        }
        if(settings.notifyinitialization)
        {
            printinbox("Initial Population Generated");
        }
        populationandfitness.clear();
        for(int i=0;i<settings.maxpopulationsize;++i)
        {
            populationandfitness.push_back({Solution(settings.numberoftasks,settings.numberofprocessors),0});
        }
    }
    void printsamplesfromcurrentgeneration()
    {
        if(!settings.printcurrentgeneration) return;
        if(settings.samplesizetoprint==0) return;
        printinbox("Generation "+std::to_string(generationnumber));
        if(settings.samplesizetoprint>settings.maxpopulationsize)
        {
            settings.samplesizetoprint=settings.maxpopulationsize;
        }
        // int nottoprint=settings.maxpopulationsize-settings.samplesizetoprint;
        // int increment=(nottoprint/settings.samplesizetoprint)+1;
        int printed=0;
        int increment=settings.maxpopulationsize/settings.samplesizetoprint;
        for(int i=0;printed<settings.samplesizetoprint;i+=increment)
        {
            auto &sol=populationandfitness[i];
            sol.first.printchromosome();
            ++printed;
            if(settings.printcurrentevaluation)
            {
                sol.second=eval.evaluate(sol.first);
                printinbox("Fitness Evaluation");
                eval.printlastevaluation();
            }
        }
    }
    void evaluateall()
    {
        for(auto &sol:populationandfitness)
        {
            sol.second=eval.evaluate(sol.first);
        }
    }
    void printselection()
    {
        if(settings.printselection)
        {
            printinbox("Solutions selected");
            for(int j=0;j<settings.selectionsize;++j)
            {
                cout<<"Solution "<<populationandfitness[j].first.getid()<<"\tRelease Time:"<<populationandfitness[j].second<<'\n';
            }
        }
    }
    void selectbyrank()
    {
        sort(populationandfitness.begin(),populationandfitness.end(),[](const auto& a, const auto& b) -> bool
        {
            return a.second < b.second;
        });
        if(settings.selectionsize>settings.maxpopulationsize)
        {
            settings.selectionsize=settings.maxpopulationsize;
        }
        printselection();
    }
    void selectbytournament()
    {
        shufflevector(populationandfitness);
        int insertat=0;
        int minfitness=INT_MAX;
        int minfitnessindex=0;
        for(int i=0;i<settings.maxpopulationsize;++i)
        {
            if(populationandfitness[i].second<minfitness)
            {
                minfitnessindex=i;
                minfitness=populationandfitness[i].second;
            }
            if(((i+1)%settings.tournamentsize==0)||(i==settings.maxpopulationsize-1))
            {
                populationandfitness[insertat++]=populationandfitness[minfitnessindex];
                minfitness=0;
                minfitness=INT_MAX;
            }
        }
        printselection();
    }
    void selectbyroulettewheel()
    {
        shufflevector(populationandfitness);
        int insertat=0;
        int fitnesssum=0;
        vector<int> ranges;
        ranges.reserve(settings.roulettesize);
        int chosenindex=0;
        int totalfitness=0;
        for(auto &p:populationandfitness)
        {
            totalfitness+=p.second;
        }
        for(int i=0;i<settings.maxpopulationsize;++i)
        {
            fitnesssum+=(totalfitness/populationandfitness[i].second);
            ranges.push_back(fitnesssum);
            if(((i+1)%settings.roulettesize==0)||(i==settings.maxpopulationsize-1))
            {
                int randomselect=getrandom(0,fitnesssum);
                for(auto &range:ranges)
                {
                    if(randomselect<=range)
                    {
                        break;
                    }
                    ++chosenindex;
                }
                populationandfitness[insertat++]=populationandfitness[chosenindex];
                ranges.clear();
                ranges.reserve(settings.roulettesize);
            }
        }
        
        printselection();
    }
    void crossoverandmutation()
    {
        if(settings.printcrossoverandmutation)
        {
            printinbox("Generating Children by Crossover and Mutation");
        }
        int increment;
        if(settings.samplesizetoprint!=0)
        {
            increment=(settings.maxpopulationsize-settings.selectionsize)/settings.samplesizetoprint;
        }
        else
        {
            increment=settings.maxpopulationsize+1;
        }
        if(increment==0)
        {
            increment=1;
        }
        int printed=0;
        for(int j=settings.selectionsize;j<settings.maxpopulationsize;++j)
        {
            bool shouldweprinthis=settings.printcrossoverandmutation && ( j % increment == 0) && (printed < settings.samplesizetoprint);
            if(settings.selectionsize<2)
            {
                break;
            }
            int select1=getrandom(0,settings.selectionsize-1);
            int select2=getrandom(0,settings.selectionsize-1);
            while(select2==select1)
            {
                select2=getrandom(0,settings.selectionsize-1);
            }
            auto &parent1=populationandfitness[select1].first;
            auto &parent2=populationandfitness[select2].first;
            populationandfitness[j].first=parent1.crossover(parent2);
            if(shouldweprinthis)
            {
                printinbox("After crossing Solution "+std::to_string(parent1.getid())+" with Solution "+std::to_string(parent2.getid()));
                parent1.printchromosome();
                cout<<"    |\n";
                cout<<"    |\n";
                cout<<"====+====\n";
                cout<<"    |\n";
                cout<<"    |\n";
                parent2.printchromosome();
                printinbox("We get");
                cout<<"   |\n";
                cout<<"\\  |  /\n";
                cout<<" \\ | /\n";
                cout<<"  \\|/\n";
                populationandfitness[j].first.printchromosome();
            }
            eval.mutatesolution(populationandfitness[j].first);
            if(shouldweprinthis)
            {
                printinbox("After mutation");
                populationandfitness[j].first.printchromosome();
                ++printed;
            }
        }
    }
    void calculatenextgeneration()
    {
        if(settings.printcurrentgeneration)
        {
            printsamplesfromcurrentgeneration();
        }
        evaluateall();
        if(settings.mode==SelectionMode::rank)
        {
            selectbyrank();
        }
        else if(settings.mode==SelectionMode::tournament)
        {
            selectbytournament();
        }
        else if(settings.mode==SelectionMode::roulettewheel)
        {
            selectbyroulettewheel();
        }
        else
        {
            cout<<"Something went wrong: Invalid selection mode, aborting\n";
            return;
        }
        crossoverandmutation();
    }
    void runforgenerations(int numberofgenerations)
    {
        while(numberofgenerations--)
        {
            calculatenextgeneration();
        }
    }
    int changealgorithmsettings()
    {
        Menu menu({},"Change algorithm settings");
        int boxselection=0;
        while(true)
        {
            vector<string> newoptions{
            "Back",
            "Number Of Tasks:"+std::to_string(settings.numberoftasks),
            "Number Of Processors:"+std::to_string(settings.numberofprocessors),
            "Minimum Processing Time:"+std::to_string(settings.minprocessingtime),
            "Maximum Processing Time:"+std::to_string(settings.maxprocessingtime),
            "Maximum Population Size:"+std::to_string(settings.maxpopulationsize),
            "Selection Size:"+std::to_string(settings.selectionsize)+(settings.mode==SelectionMode::rank?" |>Current Mode":" !!Not available in current mode!!"),
            "Tournament Size(Recalculates selection size):"+std::to_string(settings.tournamentsize)+(settings.mode==SelectionMode::tournament?" |>Current Mode":" !!Not available in current mode!!"),
            "Roulette Size(Recalculates selection size):"+std::to_string(settings.roulettesize)+(settings.mode==SelectionMode::roulettewheel?" |>Current Mode":" !!Not available in current mode!!"),
            "Number Of Generations:"+std::to_string(settings.numberofgenerations),
            };
            menu.options=newoptions;
            int selection= menu.startmenuandgetoption(boxselection);
            if(selection<=0)
            {
                return selection;
            }
            switch (selection)
            {
            case 1:
                cout<<"Enter new value:";
                cin>>settings.numberoftasks;
                eval=Evaluator(settings.numberoftasks,settings.numberofprocessors,settings.minprocessingtime,settings.maxprocessingtime);
                break;
            case 2:
                cout<<"Enter new value:";
                cin>>settings.numberofprocessors;
                eval=Evaluator(settings.numberoftasks,settings.numberofprocessors,settings.minprocessingtime,settings.maxprocessingtime);
                break;
            case 3:
                cout<<"Enter new value:";
                cin>>settings.minprocessingtime;
                eval=Evaluator(settings.numberoftasks,settings.numberofprocessors,settings.minprocessingtime,settings.maxprocessingtime);
                break;
            case 4:
                cout<<"Enter new value:";
                cin>>settings.maxprocessingtime;
                eval=Evaluator(settings.numberoftasks,settings.numberofprocessors,settings.minprocessingtime,settings.maxprocessingtime);
                break;
            case 5:
                cout<<"Enter new value:";
                cin>>settings.maxpopulationsize;
                if(settings.mode==SelectionMode::tournament)
                {
                    settings.selectionsize=(settings.maxpopulationsize/settings.tournamentsize)+((settings.maxpopulationsize%settings.tournamentsize)!=0);
                }
                if(settings.mode==SelectionMode::roulettewheel)
                {
                    settings.selectionsize=(settings.maxpopulationsize/settings.roulettesize)+((settings.maxpopulationsize%settings.roulettesize)!=0);
                }
                break;
            case 6:
                if(settings.mode==SelectionMode::rank)
                {
                    cout<<"Enter new value:";
                    cin>>settings.selectionsize;
                }
                else
                {
                    cout<<"Not available in this selection mode\n";
                }
                break;
            case 7:
                if(settings.mode==SelectionMode::tournament)
                {
                    cout<<"Enter new value:";
                    cin>>settings.tournamentsize;
                    if(settings.tournamentsize<=0)
                    {
                        settings.tournamentsize=1;
                    }
                    settings.selectionsize=(settings.maxpopulationsize/settings.tournamentsize)+((settings.maxpopulationsize%settings.tournamentsize)!=0);
                }
                else
                {
                    cout<<"Not available in this selection mode\n";
                }
                break;
            case 8:
                if(settings.mode==SelectionMode::roulettewheel)
                {
                    cout<<"Enter new value:";
                    cin>>settings.roulettesize;
                    if(settings.roulettesize<=0)
                    {
                        settings.roulettesize=1;
                    }
                    settings.selectionsize=(settings.maxpopulationsize/settings.roulettesize)+((settings.maxpopulationsize%settings.roulettesize)!=0);
                }
                else
                {
                    cout<<"Not available in this selection mode\n";
                }
                break;
            case 9:
                cin>>settings.numberofgenerations;
                break;
            

            default:
                return 0;
            }
        }
        return 0;
    }
    int changealgorithmdisplaysettings()
    {
        Menu menu({},"Change algorithm display settings");
        string yes("Yes"),no("No");
        int boxselection=0;
        while(true)
        {
            vector<string> newoptions{
            "Back",
            "(Toggle) Notify when Initial Population is Created:"+(settings.notifyinitialization?yes:no),
            "(Toggle) Print Execution Matrix:"+(settings.printexecutionmatrix?yes:no),
            "(Toggle) Print Current Generation:"+(settings.printcurrentgeneration?yes:no),
            "(Toggle) Print Current Evaluation:"+(settings.printcurrentevaluation?yes:no),
            "(Toggle) Print Selection:"+(settings.printselection?yes:no),
            "(Toggle) Print Crossover and Mutation:"+(settings.printcrossoverandmutation?yes:no),
            "Sample size to print:"+std::to_string(settings.samplesizetoprint),
            "Turn off all printing",
            "Turn on all printing"
            };
            menu.options=newoptions;
            int selection= menu.startmenuandgetoption(boxselection);
            switch (selection)
            {
            case -1:
                return -1;
            case 1:
                settings.notifyinitialization=(!settings.notifyinitialization);
                break;
            case 2:
                settings.printexecutionmatrix=(!settings.printexecutionmatrix);
                break;
            case 3:
                settings.printcurrentgeneration=(!settings.printcurrentgeneration);
                break;
            case 4:
                settings.printcurrentevaluation=(!settings.printcurrentevaluation);
                break;
            case 5:
                settings.printselection=(!settings.printselection);
                break;
            case 6:
                settings.printcrossoverandmutation=(!settings.printcrossoverandmutation);
                break;
            case 7:
                cout<<"Enter new value:\n";
                cin>>settings.samplesizetoprint;
                break;
            case 8:
                settings.turnoffallprinting();
                break;
            case 9:
                settings.turnonallprinting();
                break;

            default:
                return 0;
            }
        }
        return 0;
    }
    int startnewrun()
    {
        generateinitialpopulation();
        Menu menu({"Back","Run for more generations","Change population size","Change algorithm display settings"});
        int boxselection=0;
        int generationstorun=settings.numberofgenerations;
        
        runforgenerations(generationstorun);
        
        menu.menuname="Ran for "+std::to_string(generationstorun)+" generations";
        while(true)
        {
            int selection= menu.startmenuandgetoption(boxselection);
            switch (selection)
            {
            case -1:
                return -1;
            case 0:
                return 0;
            case 1:
                cout<<"How many generations to run?\n";
                cin>>generationstorun;
                runforgenerations(generationstorun);
                menu.menuname="Ran for "+std::to_string(generationstorun)+" generations";
                break;
            case 2:
                cout<<"Enter new population size(Current:"<<settings.maxpopulationsize<<"):";
                cin>>settings.maxpopulationsize;
                cout<<"Enter new selection size less than population size(Current:"<<settings.selectionsize<<"):";
                cin>>settings.selectionsize;
                cout<<"Set new population as "<<settings.maxpopulationsize<<"and selection size as "<<settings.selectionsize;
                populationandfitness.resize(settings.maxpopulationsize,{Solution(settings.numberoftasks,settings.numberofprocessors),0});
                menu.menuname="Changed population size";
                break;
            case 3:
                if(changealgorithmdisplaysettings()==-1)
                {
                    return -1;
                }
                menu.menuname="Changed display settings";
                break;
            case 4:

            default:
                return 0;
            }
        }
        return 0;
    }
    int changeselectionmode()
    {
        Menu menu({},"Current mode:"+settings.getcurrentselectionmode());
        string yes("Yes"),no("No");
        int boxselection=0;
        while(true)
        {
            menu.menuname="Current mode:"+settings.getcurrentselectionmode();
            menu.options={
            "Back",
            "Rank",
            "Tournament",
            "Roulette Wheel"
            };
            int selection= menu.startmenuandgetoption(boxselection);
            switch (selection)
            {
            case -1:
                return -1;
            case 1:
                settings.mode=SelectionMode::rank;
                break;
            case 2:
                settings.mode=SelectionMode::tournament;
                break;
            case 3:
                settings.mode=SelectionMode::roulettewheel;
                break;

            default:
                return 0;
            }
        }
        return 0;
    }
    void mainmenu()
    {
        Menu menu({"Change algorithm settings","Change algorithm display settings","Start new run","Change selection mode","Exit"},"Main Menu");
        int boxselection=0;
        while(true)
        {
            int selection= menu.startmenuandgetoption(boxselection);
            switch (selection)
            {
            case 0:
                if(changealgorithmsettings()==-1)
                {
                    return;
                }
                break;
            case 1:
                if(changealgorithmdisplaysettings()==-1)
                {
                    return;
                }
                break;
            case 2:
                if(startnewrun()==-1)
                {
                    return;
                }
                break;
            case 3:
                if(changeselectionmode()==-1)
                {
                    return;
                }
                break;
                
            default:
                return;
            }
        }
    }
};
int AlgorithmManager::generationnumber=1;
int main()
{
    initializerandom();
    Settings settings;
    AlgorithmManager a(settings);
    a.mainmenu();
    return 0;
}