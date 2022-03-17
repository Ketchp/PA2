#pragma once

#include <iosfwd>

namespace my_std
{
	class string
	{
		public:
			// ==============================================
			// ==================Constructors================
			// vytvoreni prazdneho retezce
			// vytvoreni kopie retezce predaneho jako const char *
			// ==============================================
			// ==================Capacity====================
			size_t size ( void ) const;
			bool empty ( void ) const;
			// ==============================================
			// ==============Element access==================
			const char & operator [] ( size_t ) const;
			char & operator [] ( size_t ); 
			const char * data ( void ) const;
			char * data ( void );
			const char * c_str ( void ) const;
			// ==============================================
			// ===============Compare========================
			friend bool operator <  ( const string &, const string & );
			friend bool operator <= ( const string &, const string & );
			friend bool operator == ( const string &, const string & );
			friend bool operator != ( const string &, const string & );
			friend bool operator >= ( const string &, const string & );
			friend bool operator >  ( const string &, const string & );
			// ==============================================
			// ================operations====================
			void push_back ( char );
			void pop_back ( void );
			void clear ( void );
			string & operator += ( const string & );
			string operator + ( const string & ) const;
		private:
			// ==============================================
			// ===============private vars===================
	};

	bool operator <  ( const string &, const string & );
	bool operator <= ( const string &, const string & );
	bool operator == ( const string &, const string & );
	bool operator != ( const string &, const string & );
	bool operator >= ( const string &, const string & );
	bool operator >  ( const string &, const string & );
	std::ostream & operator << ( std::ostream & , const string & );
	std::istream & operator >> ( std::istream & , string & );
}
