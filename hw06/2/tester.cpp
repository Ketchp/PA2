#include "test.hpp"

static bool whitespaceMatch( const string &a, const string &b )
{
  const static string whiteSpaces( " \n\t\r" );
  for( auto ita = a.begin(),
               itb = b.begin(); true;
       ++ita,
               ++itb)
  {
    while(  ita != a.end() && whiteSpaces.find( *ita ) != string::npos )
      ++ita;

    while( itb != b.end() && whiteSpaces.find( *itb ) != string::npos )
      ++itb;

    if( ita == a.end() && itb == b.end() )
      return true;

    if( ( ita == a.end() ) != ( itb == b.end() ) )
      return false;

    if( *ita != *itb )
      return false;
  }
}

template <typename T_>
static bool whitespaceMatch( const T_ &x, const string &ref )
{
  ostringstream oss;
  oss << x;
  cout << oss.str();
  return whitespaceMatch ( oss . str (), ref );
}

int main ()
{
  CDataTypeStruct  a = CDataTypeStruct () .
          addField ( "m_Length", CDataTypeInt () ) .
          addField ( "m_Status", CDataTypeEnum () .
          add ( "NEW" ) .
          add ( "FIXED" ) .
          add ( "BROKEN" ) .
          add ( "DEAD" ) ).
          addField ( "m_Ratio", CDataTypeDouble () );

  CDataTypeStruct b = CDataTypeStruct () .
          addField ( "m_Length", CDataTypeInt () ) .
          addField ( "m_Status", CDataTypeEnum () .
          add ( "NEW" ) .
          add ( "FIXED" ) .
          add ( "BROKEN" ) .
          add ( "READY" ) ).
          addField ( "m_Ratio", CDataTypeDouble () );

  CDataTypeStruct c =  CDataTypeStruct () .
          addField ( "m_First", CDataTypeInt () ) .
          addField ( "m_Second", CDataTypeEnum () .
          add ( "NEW" ) .
          add ( "FIXED" ) .
          add ( "BROKEN" ) .
          add ( "DEAD" ) ).
          addField ( "m_Third", CDataTypeDouble () );

  CDataTypeStruct  d = CDataTypeStruct () .
          addField ( "m_Length", CDataTypeInt () ) .
          addField ( "m_Status", CDataTypeEnum () .
          add ( "NEW" ) .
          add ( "FIXED" ) .
          add ( "BROKEN" ) .
          add ( "DEAD" ) ).
          addField ( "m_Ratio", CDataTypeInt () );

  assert ( whitespaceMatch ( a, "struct\n"
                                "{\n"
                                "  int m_Length;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    DEAD\n"
                                "  } m_Status;\n"
                                "  double m_Ratio;\n"
                                "}") );

  assert ( whitespaceMatch ( b, "struct\n"
                                "{\n"
                                "  int m_Length;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    READY\n"
                                "  } m_Status;\n"
                                "  double m_Ratio;\n"
                                "}") );

  assert ( whitespaceMatch ( c, "struct\n"
                                "{\n"
                                "  int m_First;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    DEAD\n"
                                "  } m_Second;\n"
                                "  double m_Third;\n"
                                "}") );

  assert ( whitespaceMatch ( d, "struct\n"
                                "{\n"
                                "  int m_Length;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    DEAD\n"
                                "  } m_Status;\n"
                                "  int m_Ratio;\n"
                                "}") );

  assert ( a != b );
  assert ( a == c );
  assert ( a != d );
  assert ( a != CDataTypeInt() );
  assert ( whitespaceMatch ( a . field ( "m_Status" ), "enum\n"
                                                       "{\n"
                                                       "  NEW,\n"
                                                       "  FIXED,\n"
                                                       "  BROKEN,\n"
                                                       "  DEAD\n"
                                                       "}") );

  b . addField ( "m_Other", a );

  a . addField ( "m_Sum", CDataTypeInt ());

  assert ( whitespaceMatch ( a, "struct\n"
                                "{\n"
                                "  int m_Length;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    DEAD\n"
                                "  } m_Status;\n"
                                "  double m_Ratio;\n"
                                "  int m_Sum;\n"
                                "}") );

  assert ( whitespaceMatch ( b, "struct\n"
                                "{\n"
                                "  int m_Length;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    READY\n"
                                "  } m_Status;\n"
                                "  double m_Ratio;\n"
                                "  struct\n"
                                "  {\n"
                                "    int m_Length;\n"
                                "    enum\n"
                                "    {\n"
                                "      NEW,\n"
                                "      FIXED,\n"
                                "      BROKEN,\n"
                                "      DEAD\n"
                                "    } m_Status;\n"
                                "    double m_Ratio;\n"
                                "  } m_Other;\n"
                                "}") );

  assert ( whitespaceMatch ( b . field ( "m_Other" ), "struct\n"
                                                      "{\n"
                                                      "  int m_Length;\n"
                                                      "  enum\n"
                                                      "  {\n"
                                                      "    NEW,\n"
                                                      "    FIXED,\n"
                                                      "    BROKEN,\n"
                                                      "    DEAD\n"
                                                      "  } m_Status;\n"
                                                      "  double m_Ratio;\n"
                                                      "}") );

  assert ( whitespaceMatch ( b . field ( "m_Other" ) . field ( "m_Status" ), "enum\n"
                                                                             "{\n"
                                                                             "  NEW,\n"
                                                                             "  FIXED,\n"
                                                                             "  BROKEN,\n"
                                                                             "  DEAD\n"
                                                                             "}") );

  assert ( a . getSize () == 20 );
  assert ( b . getSize () == 32 );
  b . addField ( "m_Other1", b );
  b . addField ( "m_Other2", b );
  b . addField ( "m_Other3", b );
  assert ( b . field ( "m_Other3" ) . field ( "m_Other2" ) . field ( "m_Other1" ) == b . field ( "m_Other1" ) );

  assert ( b . getSize () == 256);

  assert ( whitespaceMatch ( b, "struct\n"
                                "{\n"
                                "  int m_Length;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    READY\n"
                                "  } m_Status;\n"
                                "  double m_Ratio;\n"
                                "  struct\n"
                                "  {\n"
                                "    int m_Length;\n"
                                "    enum\n"
                                "    {\n"
                                "      NEW,\n"
                                "      FIXED,\n"
                                "      BROKEN,\n"
                                "      DEAD\n"
                                "    } m_Status;\n"
                                "    double m_Ratio;\n"
                                "  } m_Other;\n"
                                "  struct\n"
                                "  {\n"
                                "    int m_Length;\n"
                                "    enum\n"
                                "    {\n"
                                "      NEW,\n"
                                "      FIXED,\n"
                                "      BROKEN,\n"
                                "      READY\n"
                                "    } m_Status;\n"
                                "    double m_Ratio;\n"
                                "    struct\n"
                                "    {\n"
                                "      int m_Length;\n"
                                "      enum\n"
                                "      {\n"
                                "        NEW,\n"
                                "        FIXED,\n"
                                "        BROKEN,\n"
                                "        DEAD\n"
                                "      } m_Status;\n"
                                "      double m_Ratio;\n"
                                "    } m_Other;\n"
                                "  } m_Other1;\n"
                                "  struct\n"
                                "  {\n"
                                "    int m_Length;\n"
                                "    enum\n"
                                "    {\n"
                                "      NEW,\n"
                                "      FIXED,\n"
                                "      BROKEN,\n"
                                "      READY\n"
                                "    } m_Status;\n"
                                "    double m_Ratio;\n"
                                "    struct\n"
                                "    {\n"
                                "      int m_Length;\n"
                                "      enum\n"
                                "      {\n"
                                "        NEW,\n"
                                "        FIXED,\n"
                                "        BROKEN,\n"
                                "        DEAD\n"
                                "      } m_Status;\n"
                                "      double m_Ratio;\n"
                                "    } m_Other;\n"
                                "    struct\n"
                                "    {\n"
                                "      int m_Length;\n"
                                "      enum\n"
                                "      {\n"
                                "        NEW,\n"
                                "        FIXED,\n"
                                "        BROKEN,\n"
                                "        READY\n"
                                "      } m_Status;\n"
                                "      double m_Ratio;\n"
                                "      struct\n"
                                "      {\n"
                                "        int m_Length;\n"
                                "        enum\n"
                                "        {\n"
                                "          NEW,\n"
                                "          FIXED,\n"
                                "          BROKEN,\n"
                                "          DEAD\n"
                                "        } m_Status;\n"
                                "        double m_Ratio;\n"
                                "      } m_Other;\n"
                                "    } m_Other1;\n"
                                "  } m_Other2;\n"
                                "  struct\n"
                                "  {\n"
                                "    int m_Length;\n"
                                "    enum\n"
                                "    {\n"
                                "      NEW,\n"
                                "      FIXED,\n"
                                "      BROKEN,\n"
                                "      READY\n"
                                "    } m_Status;\n"
                                "    double m_Ratio;\n"
                                "    struct\n"
                                "    {\n"
                                "      int m_Length;\n"
                                "      enum\n"
                                "      {\n"
                                "        NEW,\n"
                                "        FIXED,\n"
                                "        BROKEN,\n"
                                "        DEAD\n"
                                "      } m_Status;\n"
                                "      double m_Ratio;\n"
                                "    } m_Other;\n"
                                "    struct\n"
                                "    {\n"
                                "      int m_Length;\n"
                                "      enum\n"
                                "      {\n"
                                "        NEW,\n"
                                "        FIXED,\n"
                                "        BROKEN,\n"
                                "        READY\n"
                                "      } m_Status;\n"
                                "      double m_Ratio;\n"
                                "      struct\n"
                                "      {\n"
                                "        int m_Length;\n"
                                "        enum\n"
                                "        {\n"
                                "          NEW,\n"
                                "          FIXED,\n"
                                "          BROKEN,\n"
                                "          DEAD\n"
                                "        } m_Status;\n"
                                "        double m_Ratio;\n"
                                "      } m_Other;\n"
                                "    } m_Other1;\n"
                                "    struct\n"
                                "    {\n"
                                "      int m_Length;\n"
                                "      enum\n"
                                "      {\n"
                                "        NEW,\n"
                                "        FIXED,\n"
                                "        BROKEN,\n"
                                "        READY\n"
                                "      } m_Status;\n"
                                "      double m_Ratio;\n"
                                "      struct\n"
                                "      {\n"
                                "        int m_Length;\n"
                                "        enum\n"
                                "        {\n"
                                "          NEW,\n"
                                "          FIXED,\n"
                                "          BROKEN,\n"
                                "          DEAD\n"
                                "        } m_Status;\n"
                                "        double m_Ratio;\n"
                                "      } m_Other;\n"
                                "      struct\n"
                                "      {\n"
                                "        int m_Length;\n"
                                "        enum\n"
                                "        {\n"
                                "          NEW,\n"
                                "          FIXED,\n"
                                "          BROKEN,\n"
                                "          READY\n"
                                "        } m_Status;\n"
                                "        double m_Ratio;\n"
                                "        struct\n"
                                "        {\n"
                                "          int m_Length;\n"
                                "          enum\n"
                                "          {\n"
                                "            NEW,\n"
                                "            FIXED,\n"
                                "            BROKEN,\n"
                                "            DEAD\n"
                                "          } m_Status;\n"
                                "          double m_Ratio;\n"
                                "        } m_Other;\n"
                                "      } m_Other1;\n"
                                "    } m_Other2;\n"
                                "  } m_Other3;\n"
                                "}" ) );

  try
  {
    a . addField ( "m_Status", CDataTypeInt () );
    assert ( "addField: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Duplicate field: m_Status"sv );
  }

  try
  {
    cout << a . field ( "m_Fail" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Unknown field: m_Fail"sv );
  }

  try
  {
    cout << a . field ( "m_Length" ) . field ( "m_Foo" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    cout << endl <<  e.what() << endl;
    assert ( whitespaceMatch ( e . what (), "Cannot use field() for type: int" ));
  }

  try
  {
    cout << a . field ( "m_Status" ) . field ( "m_Foo" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( whitespaceMatch ( e . what (), "Cannot use field() for type: enum\n"
                                            "{\n"
                                            "  NEW,\n"
                                            "  FIXED,\n"
                                            "  BROKEN,\n"
                                            "  DEAD\n"
                                            "}" ));
  }

  try
  {
    CDataTypeEnum en;
    en . add ( "FIRST" ) .
            add ( "SECOND" ) .
            add ( "FIRST" );
    assert ( "add: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( e . what () == "Duplicate enum value: FIRST"sv );
  }

  CDataTypeArray ar1 ( 10, CDataTypeInt () );
  assert ( whitespaceMatch ( ar1, "int[10]") );
  assert ( whitespaceMatch ( ar1 . element (), "int") );
  CDataTypeArray ar2 ( 11, ar1 );
  assert ( whitespaceMatch ( ar2, "int[11][10]") );
  assert ( whitespaceMatch ( ar2 . element (), "int[10]") );
  assert ( whitespaceMatch ( ar2 . element () . element (), "int") );
  CDataTypeArray ar3 ( 10, CDataTypeArray ( 20, CDataTypePtr ( CDataTypeInt () ) ) );
  assert ( whitespaceMatch ( ar3, "int*[10][20]") );
  assert ( whitespaceMatch ( ar3 . element (), "int*[20]") );
  assert ( whitespaceMatch ( ar3 . element () . element (), "int*") );
  assert ( whitespaceMatch ( ar3 . element () . element () . element (), "int") );
  CDataTypePtr ar4  ( ar1 . element () );
  assert ( whitespaceMatch ( ar4, "int*") );
  assert ( whitespaceMatch ( ar4 . element (), "int") );
  CDataTypePtr ar5  ( b . field ( "m_Status" ) );
  assert ( whitespaceMatch ( ar5, "enum\n"
                                  "{\n"
                                  "  NEW,\n"
                                  "  FIXED,\n"
                                  "  BROKEN,\n"
                                  "  READY\n"
                                  "}*") );
  assert ( whitespaceMatch ( ar5 . element (), "enum\n"
                                               "{\n"
                                               "  NEW,\n"
                                               "  FIXED,\n"
                                               "  BROKEN,\n"
                                               "  READY\n"
                                               "}") );
  CDataTypePtr ar6 ( ar3 . element () . element () );
  assert ( whitespaceMatch ( ar6, "int**") );
  assert ( whitespaceMatch ( ar6 . element (), "int*") );
  assert ( whitespaceMatch ( ar6 . element () . element (), "int") );
  CDataTypePtr ar7 ( CDataTypeArray ( 50, ar6 ) );
  assert ( whitespaceMatch ( ar7, "int**(*)[50]") );
  assert ( whitespaceMatch ( ar7 . element (), "int**[50]") );
  assert ( whitespaceMatch ( ar7 . element () . element (), "int**") );
  assert ( whitespaceMatch ( ar7 . element () . element () . element (), "int*") );
  assert ( whitespaceMatch ( ar7 . element () . element () . element () . element (), "int") );
  CDataTypeArray ar8 ( 25, ar7 );
  assert ( whitespaceMatch ( ar8, "int**(*[25])[50]") );
  CDataTypePtr ar9 ( ar8 );
  assert ( whitespaceMatch ( ar9, "int**(*(*)[25])[50]") );
  a . addField ( "m_Ar1", ar1 ) .
          addField ( "m_Ar2", ar2 ) .
          addField ( "m_Ar3", ar3 ) .
          addField ( "m_Ar4", ar4 ) .
          addField ( "m_Ar5", ar5 ) .
          addField ( "m_Ar6", ar6 ) .
          addField ( "m_Ar7", ar7 ) .
          addField ( "m_Ar8", ar8 ) .
          addField ( "m_Ar9", ar9 );
  assert ( whitespaceMatch ( a, "struct\n"
                                "{\n"
                                "  int m_Length;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    DEAD\n"
                                "  } m_Status;\n"
                                "  double m_Ratio;\n"
                                "  int m_Sum;\n"
                                "  int m_Ar1[10];\n"  // "int.[10]"
                                "  int m_Ar2[11][10];\n"  // "int.[11][10]"
                                "  int* m_Ar3[10][20];\n" // "int*.[10][20]"
                                "  int* m_Ar4;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    READY\n"
                                "  }* m_Ar5;\n"
                                "  int** m_Ar6;\n"               // "int**."
                                "  int**(* m_Ar7)[50];\n"        // "int**(*.)[50]"
                                "  int**(* m_Ar8[25])[50];\n"    // "int**(*.[25])[50]"
                                "  int**(*(* m_Ar9)[25])[50];\n" // "int**(*(*.)[25])[50]"
                                "}") );
  a . addField ( "m_Madness", CDataTypeArray ( 40, CDataTypePtr ( a ) ) );
  assert ( whitespaceMatch ( a, "struct\n"
                                "{\n"
                                "  int m_Length;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    DEAD\n"
                                "  } m_Status;\n"
                                "  double m_Ratio;\n"
                                "  int m_Sum;\n"
                                "  int m_Ar1[10];\n"
                                "  int m_Ar2[11][10];\n"
                                "  int* m_Ar3[10][20];\n"
                                "  int* m_Ar4;\n"
                                "  enum\n"
                                "  {\n"
                                "    NEW,\n"
                                "    FIXED,\n"
                                "    BROKEN,\n"
                                "    READY\n"
                                "  }* m_Ar5;\n"
                                "  int** m_Ar6;\n"
                                "  int**(* m_Ar7)[50];\n"
                                "  int**(* m_Ar8[25])[50];\n"
                                "  int**(*(* m_Ar9)[25])[50];\n"
                                "  struct\n"
                                "  {\n"
                                "    int m_Length;\n"
                                "    enum\n"
                                "    {\n"
                                "      NEW,\n"
                                "      FIXED,\n"
                                "      BROKEN,\n"
                                "      DEAD\n"
                                "    } m_Status;\n"
                                "    double m_Ratio;\n"
                                "    int m_Sum;\n"
                                "    int m_Ar1[10];\n"
                                "    int m_Ar2[11][10];\n"
                                "    int* m_Ar3[10][20];\n"
                                "    int* m_Ar4;\n"
                                "    enum\n"
                                "    {\n"
                                "      NEW,\n"
                                "      FIXED,\n"
                                "      BROKEN,\n"
                                "      READY\n"
                                "    }* m_Ar5;\n"
                                "    int** m_Ar6;\n"
                                "    int**(* m_Ar7)[50];\n"
                                "    int**(* m_Ar8[25])[50];\n"
                                "    int**(*(* m_Ar9)[25])[50];\n"
                                "  }* m_Madness[40];\n"
                                "}") );
  assert ( a . field ( "m_Madness" ) . element () . element () . field ( "m_Ar9" ) == a . field ( "m_Ar9" ));
  assert ( a . field ( "m_Madness" ) . element () . element () . field ( "m_Ar9" ) != a . field ( "m_Ar8" ));
  try
  {
    cout << ar2 . field ( "m_Foo" ) << endl;
    assert ( "field: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( whitespaceMatch ( e . what (), "Cannot use field() for type: int[11][10]" ));
  }

  try
  {
    cout << c . element () << endl;
    assert ( "element: missing exception!" == nullptr );
  }
  catch ( const invalid_argument & e )
  {
    assert ( whitespaceMatch ( e . what (), "Cannot use element() for type: struct\n"
                                            "{\n"
                                            "  int m_First;\n"
                                            "  enum\n"
                                            "  {\n"
                                            "    NEW,\n"
                                            "    FIXED,\n"
                                            "    BROKEN,\n"
                                            "    DEAD\n"
                                            "  } m_Second;\n"
                                            "  double m_Third;\n"
                                            "}" ));
  }
  cout << endl << "hello" << endl;
  return EXIT_SUCCESS;
}
