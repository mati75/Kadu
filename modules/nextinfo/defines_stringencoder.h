#define  NEXTINFO_ESCAPES  typedef QPair<QString,QString> EscapesPair;     \
                           typedef QList<EscapesPair> Escapes;             \
                           Escapes escapes;                                \
                           Escapes::Iterator escapesit;                    \
                           escapes.append( EscapesPair( "\\", "\\\\" ) );  \
                           escapes.append( EscapesPair( "\n", "\\n"  ) );




#define  NEXTINFO_ENCODESTRING                                                   \
static QString encodeString( QString s )                                         \
{                                                                                \
	NEXTINFO_ESCAPES                                                               \
	uint k = 0;                                                                    \
	while( k < s.length() )                                                        \
	{                                                                              \
		for( escapesit = escapes.begin(); escapesit != escapes.end(); ++escapesit )  \
		{                                                                            \
			if( s.mid( k, (*escapesit).first.length() ) == (*escapesit).first )        \
			{                                                                          \
				s = s.replace( k, (*escapesit).first.length(), (*escapesit).second );    \
				k += (*escapesit).second.length() - 1;                                   \
				break;                                                                   \
			}                                                                          \
		}                                                                            \
		k++;                                                                         \
	}                                                                              \
	return s;                                                                      \
}


#define  NEXTINFO_DECODESTRING                                                   \
static QString decodeString( QString s )                                         \
{                                                                                \
	NEXTINFO_ESCAPES                                                               \
	uint k = 0;                                                                    \
	while( k < s.length() )                                                        \
	{                                                                              \
		for( escapesit = escapes.begin(); escapesit != escapes.end(); ++escapesit )  \
		{                                                                            \
			if( s.mid( k, (*escapesit).second.length() ) == (*escapesit).second )      \
			{                                                                          \
				s = s.replace( k, (*escapesit).second.length(), (*escapesit).first );    \
				k += (*escapesit).first.length() - 1;                                    \
				break;                                                                   \
			}                                                                          \
		}                                                                            \
		k++;                                                                         \
	}                                                                              \
	return s;                                                                      \
}
