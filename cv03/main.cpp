#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

struct person
{
    string name, surname, number;
    string line;
};

bool report( const string &fileName, ostream &out )
{
    ifstream file( fileName );
    if( !file ) return false;
    vector<person> records;
    string line;
    while( true )
    {
        line = "";
        getline( file, line );
        if( line.empty() ) break;

        stringstream record(line);

        person newPerson;

        string rest;
        record >> newPerson.name >> newPerson.surname >> newPerson.number >> rest;
//        cout << newPerson.name << ":" << newPerson.surname
//        << ":" << newPerson.number << ":" << rest << ":" << endl;
        if( !rest.empty() || newPerson.number.size() != 9 ) return false;
        if( find_if( newPerson.number.begin(), newPerson.number.end(), []( unsigned char c ){
            return !isdigit(c);
        } ) != newPerson.number.end() ) return false;
        if( newPerson.number[0] == '0' ) return false;

        newPerson.line = line;
        records.push_back( newPerson );
    }

    if( file.eof() ) return false;

    while( true )
    {
        line = "";
        getline( file, line );
        if( file.eof() && line.empty() ) break;

        int count = 0;
        for( person p: records )
        {
            if( !p.name.compare( line ) || !p.surname.compare( line ) )
            {
                count++;
                out << p.name + " " + p.surname + " " + p.number + "\n";
            }
        }
        out << "-> " << count << "\n";
    }

    return true;
}

#ifndef __PROGTEST__
int main()
{
    cout << report("./tests/test0_in.txt", cout) << endl;
    cout << report("./tests/test1_in.txt", cout) << endl;
    cout << report("./tests/test2_in.txt", cout) << endl;
    cout << report("./tests/test3_in.txt", cout) << endl;
}
#endif /*__PROGTEST__*/