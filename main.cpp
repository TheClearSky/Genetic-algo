#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>

using std::cout;
using std::cin;
using std::vector;
using std::string;
using std::max;

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
void printinbox(const string message)
{
    cout<<'+';
    printrepeatingchars('-',message.length());
    cout<<"+\n|"<<message<<"|\n+";
    printrepeatingchars('-',message.length());
    cout<<"+\n";
}

//a solution of the problem
class Solution{
    
    int numberoftasks;
    int numberofprocessors;
    vector<int> chromosome;

    vector<int> maxcellwidth; //for UI
    string label1="Tasks:";
    string label2="Processors:";
    int leftspacing=1;
    int rightspacing=1;

    public:
    Solution(int totaltasks,int totalprocessors,int leftspace=1,int rightspace=1)
    {
        leftspacing=leftspace;
        rightspacing=rightspace;
        numberoftasks=totaltasks;
        numberofprocessors=totalprocessors;
        chromosome.resize(numberoftasks);
        maxcellwidth.resize(numberoftasks);
        int i=1;
        for(auto &gene:chromosome)
        {
            gene=getrandom(1,numberofprocessors);
            maxcellwidth[i-1]=digitsinint(max(i,gene));
            ++i;
        }
    }
    void printchromosome()
    {
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
};
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
    int evaluate(Solution sol)
    {
        vector<int> releasetimes(numberofprocessors);
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
int main()
{
    initializerandom();
    int numberoftasks=12;
    int numberofprocessors=4;
    
    int maxpopulationsize=20;
    int selectionsize=10;

    vector<Solution> population;
    for(int i=0;i<maxpopulationsize;++i)
    {
        population.push_back(Solution(numberoftasks,numberofprocessors));
    }

    printinbox("Initial Generation");
    int i=1;
    for(auto &sol:population)
    {
        string message="Solution ";
        printinbox(message+std::to_string(i));
        sol.printchromosome();
        ++i;
    }
    

    return 0;
}