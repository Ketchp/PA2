#pragma once

#include <ostream>
#include <istream>

class CFraction
{
public:
  // ==============================================
  // ====Constructors, destructors=================
  // vychozi konstruktor,
      // konstruktor s citatelem,
      // konstruktor s citatelem i jmenovatelem
  // destruktor

  CFraction( int nominator = 0, int denominator = 1 );

  // ==============================================
  // ==============Operatory+-*/===================
  friend CFraction operator+( const CFraction &lhs, const CFraction &rhs );
  friend CFraction operator-( const CFraction &lhs, const CFraction &rhs );
  friend CFraction operator*( const CFraction &lhs, const CFraction &rhs );
  friend CFraction operator/( const CFraction &lhs, const CFraction &rhs );

  // ==============================================
  // ============== Operatory@= ===================
  CFraction &operator+=( const CFraction &other );
  CFraction &operator-=( const CFraction &other );
  CFraction &operator*=( const CFraction &other );
  CFraction &operator/=( const CFraction &other );

  // ==============================================
  // ============= Operatory++-- ==================
  CFraction operator++( int );
  CFraction operator--( int );
  CFraction &operator++( void );
  CFraction &operator--( void );

  // ==============================================
  // ================ Unarni - ====================
  CFraction operator-( void ) const;

  // ==============================================
  // ============== Compare =======================
  friend bool operator< ( const CFraction &lhs, const CFraction &rhs );
  friend bool operator<=( const CFraction &lhs, const CFraction &rhs );
  friend bool operator==( const CFraction &lhs, const CFraction &rhs );
  friend bool operator!=( const CFraction &lhs, const CFraction &rhs );
  friend bool operator>=( const CFraction &lhs, const CFraction &rhs );
  friend bool operator> ( const CFraction &lhs, const CFraction &rhs );

  // ==============================================
  // ============= IO operators ===================
  friend std::ostream &operator<<( std::ostream & out, const CFraction &frac );
  friend std::istream &operator>>( std::istream & in, CFraction &frac );
private:
  // ==============================================
  // ===============private vars===================
  void normalize( void );
  int nominator, denominator;
};

template <typename T>
int sign( T val );