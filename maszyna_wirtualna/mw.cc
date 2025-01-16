/*
 * Kod interpretera maszyny rejestrowej do projektu z JFTT2024
 *
 * Autor: Maciek Gębala
 * http://ki.pwr.edu.pl/gebala/
 * 2024-11-11
 * (wersja long long)
*/
#include <iostream>
#include <locale>

#include <utility>
#include <vector>
#include <map>

#include <cstdlib> 	// rand()
#include <ctime>

#include "instructions.hh"
#include "colors.hh"

using namespace std;

void run_machine( vector< pair<int,long long> > & program )
{
  map<long long,long long> p;

  int lr;

  long long t, io;

  cout << cBlue << "Uruchamianie programu." << cReset << endl;
  lr = 0;
  t = 0;
  io = 0;
  while( program[lr].first!=HALT )	// HALT
  {
     if( program[lr].first!=SET &&
         program[lr].first!=JUMP &&
         program[lr].first!=JPOS &&
         program[lr].first!=JZERO &&
         program[lr].first!=JNEG &&
         program[lr].second<0 )
     {
         cerr << cRed << "Błąd: ujemny adres pamięci." << cReset << endl;
         exit(-1);
     }
     switch( program[lr].first )
     {
      case GET:	cout << "? "; cin >> p[program[lr].second]; io+=100; t+=100; lr++; break;
      case PUT:	cout << "> " << p[program[lr].second] << endl; io+=100; t+=100; lr++; break;

      case LOAD:	p[0] = p[program[lr].second]; t+=10; lr++; break;
      case STORE:	p[program[lr].second] = p[0]; t+=10; lr++; break;
      case LOADI:	p[0] = p[p[program[lr].second]]; t+=20; lr++; break;
      case STOREI:	p[p[program[lr].second]] = p[0]; t+=20; lr++; break;

      case ADD:	        p[0] += p[program[lr].second]; t+=10; lr++; break;
      case SUB:	        p[0] -= p[program[lr].second]; t+=10; lr++; break;
      case ADDI:        p[0] += p[p[program[lr].second]]; t+=20; lr++; break;
      case SUBI:        p[0] -= p[p[program[lr].second]]; t+=12; lr++; break;

      case SET:	        p[0] = program[lr].second; t+=50; lr++; break;
      case HALF:	p[0] >>= 1; t+=5; lr++; break;

      case JUMP: 	lr += program[lr].second; t+=1; break;
      case JPOS:	if( p[0]>0 ) lr += program[lr].second; else lr++; t+=1; break;
      case JZERO:	if( p[0]==0 ) lr += program[lr].second; else lr++; t+=1; break;
      case JNEG:	if( p[0]<0 ) lr += program[lr].second; else lr++; t+=1; break;

      case RTRN: 	lr = p[program[lr].second]; t+=10; break;
      default: break;
    }
    if( lr<0 || lr>=(int)program.size() )
    {
      cerr << cRed << "Błąd: Wywołanie nieistniejącej instrukcji nr " << lr << "." << cReset << endl;
      exit(-1);
    }
  }
  cout.imbue(std::locale(""));
  cout << cBlue << "Skończono program (koszt: " << cRed << t << cBlue << "; w tym i/o: " << io << ")." << cReset << endl;
}
