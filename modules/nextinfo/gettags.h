#ifndef GETTAGS_H
	#define GETTAGS_H




#include <math.h>

#include "defines.h"

#include "defines_stringencoder.h"
NEXTINFO_DECODESTRING




static QString getTag_address( const UserListElement &user )
{
	return user.data( "nextinfo_address" ).toString();
}


static QString getTag_city( const UserListElement &user )
{
	return user.data( "nextinfo_city" ).toString();
}


static QString getTag_sex( const UserListElement &user )
{
	int sex = user.data( "nextinfo_sex" ).toInt();
	if( sex == 1 ) return qApp->translate( "@nextinfo", "female" );
	if( sex == 2 ) return qApp->translate( "@nextinfo", "male"   );
	return "";
}


static QString getTag_sexN( const UserListElement &user )
{
	return QString().setNum( user.data( "nextinfo_sex" ).toInt() );
}


static QString getTag_phone2( const UserListElement &user )
{
	return user.data( "nextinfo_phone2" ).toString();
}


static QString getTag_email2( const UserListElement &user )
{
	return user.data( "nextinfo_email2" ).toString();
}


static QString getTag_www( const UserListElement &user )
{
	return user.data( "nextinfo_www" ).toString();
}


static QString getTag_gg2( const UserListElement &user )
{
	return user.data( "nextinfo_gg2" ).toString();
}


static QString getTag_irc( const UserListElement &user )
{
	return user.data( "nextinfo_irc" ).toString();
}


static QString getTag_tlen( const UserListElement &user )
{
	return user.data( "nextinfo_tlen" ).toString();
}


static QString getTag_wp( const UserListElement &user )
{
	return user.data( "nextinfo_wp" ).toString();
}


static QString getTag_icq( const UserListElement &user )
{
	return user.data( "nextinfo_icq" ).toString();
}


static QString getTag_birthday( const UserListElement &user )
{
	return user.data( "nextinfo_birthday" ).toString();
}


static QString getTag_nameday( const UserListElement &user )
{
	return user.data( "nextinfo_nameday" ).toString();
}


static QString getTag_interests( const UserListElement &user )
{
	QString s = decodeString( user.data( "nextinfo_interests" ).toString() );
	s = s.replace( "\n", "<br/>" );
	return s;
}


static QString getTag_notes( const UserListElement &user )
{
	QString s = decodeString( user.data( "nextinfo_notes" ).toString() );
	s = s.replace( "\n", "<br/>" );
	return s;
}


static QString getTag_photo( const UserListElement &user )
{
	return QString("") + "file://" + user.data( "nextinfo_photo" ).toString().stripWhiteSpace();
}


static QString getTag_photoimgN( const UserListElement &user, int maxwidth, int maxheight )
{
	if( ( maxwidth <= 0 ) || ( maxheight <= 0 ) )
		return "";
	QString photopath = user.data( "nextinfo_photo" ).toString().stripWhiteSpace();
	if( photopath.isNull() )
		photopath = "";
	QPixmap photopixmap;
	if( ( ! photopath.isEmpty() ) && photopixmap.load( photopath ) )
	{
		int width  = photopixmap.width();
		int height = photopixmap.height();
		if( width  <= 0 ) width  = 1;
		if( height <= 0 ) height = 1;
		int newwidth  = width;
		int newheight = height;
		if( ( width > maxwidth ) || ( height > maxheight ) )
		{
			if( ( (1.0*width)/(1.0*height) ) >= ( (1.0*maxwidth)/(1.0*maxheight) ) )  // we have to compare floats
			{
				newwidth = maxwidth;
				newheight = floor( height * newwidth/width );
			}
			else
			{
				newheight = maxheight;
				newwidth = floor( width * newheight/height );
			}
		}
		return QString("") + "<img width=\"" + QString().setNum( newwidth ) + "\" height=\"" + QString().setNum( newheight ) + "\" src=\"file://" + photopath + "\">";
	}
	return "";
}
static QString getTag_photoimg1( const UserListElement &user )
{
	int maxwidth  = config_file.readNumEntry( "NExtInfo", "PhotoSize1Width"  );
	int maxheight = config_file.readNumEntry( "NExtInfo", "PhotoSize1Height" );
	return getTag_photoimgN( user, maxwidth, maxheight );
}
static QString getTag_photoimg2( const UserListElement &user )
{
	int maxwidth  = config_file.readNumEntry( "NExtInfo", "PhotoSize2Width"  );
	int maxheight = config_file.readNumEntry( "NExtInfo", "PhotoSize2Height" );
	return getTag_photoimgN( user, maxwidth, maxheight );
}
static QString getTag_photoimg3( const UserListElement &user )
{
	int maxwidth  = config_file.readNumEntry( "NExtInfo", "PhotoSize3Width"  );
	int maxheight = config_file.readNumEntry( "NExtInfo", "PhotoSize3Height" );
	return getTag_photoimgN( user, maxwidth, maxheight );
}




#endif
