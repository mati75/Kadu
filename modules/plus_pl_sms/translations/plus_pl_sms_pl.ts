<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS><TS version="1.1" language="pl_PL">
<defaultcodec></defaultcodec>
<context>
    <name>@default</name>
    <message>
        <source>SMS Miasto Plusa Gateway</source>
        <translation type="obsolete">Bramka SMS Miasta Plusa</translation>
    </message>
    <message>
        <source>User</source>
        <translation type="obsolete">Użytkownik</translation>
    </message>
    <message>
        <source>Pass</source>
        <translation type="obsolete">Hasło</translation>
    </message>
    <message>
        <source>Request confirmatoin</source>
        <translation type="obsolete">Żądaj potwierdzenia</translation>
    </message>
    <message>
        <source>Display infos about sent SMSes</source>
        <translation type="obsolete">Wyświetlaj informacje o wysłanych SMS-ach</translation>
    </message>
    <message>
        <source>Messages to networks other than PlusGSM</source>
        <translation type="obsolete">Wiadomości do sieci innych niż PlusGSM</translation>
    </message>
    <message>
        <source>Always try to send message through Miasto Plusa SMS gateway - If you choose this option, module will each
time try to send message, even if you don&apos;t have any free SMSes to send on your account. Good choice for
people, that often send messages to other networks.

Never send messages to other networks that PlusGSM through Miasto Plusa gateway - If you choose this option,
module will alaways refuse to send message to network other than PlusGSM , so the next gateway in the
&quot;Gateways priority&quot; will be asked to send SMS. Choice good for people, that never use Miasto Plusa gateway
to send messages to nesworks other than PlusGSM.</source>
        <translation type="obsolete">Zawsze próbuj wysłać wiadomość przez bramkę Miasta Plusa - Jeśli wybierzesz tą opcję, moduł zawsze będzie
próbował wysłać wiadomość, nawet gdy nie masz darmowych SMS-ów do innych sieci na koncie. Dobry wybór dla
osób które często wysyłają SMS-y do innych sieci.

Nigdy nie wysyłaj wiadomości do sieci innych niż PlusGSM przez bramkę Miasta Plusa - Jeśli wybierzesz tą
opcję, moduł zawsze będzie odmawiał wysłania wiadomości do sieci innej niż PlusGSM, więc następna bramka
w &quot;Priorytety bramek&quot; zostanie odpytana. Wybór dobry dla osób nie wysyłających SMS-ów do sieci innych niż
PlusGSM.</translation>
    </message>
    <message>
        <source>Send messages to networks other than PlusGSM using Miasto Plusa gateway: </source>
        <translation type="obsolete">Wysyłaj SMS-y do sieci innej niz PlusGSM przez Miasto Plusa: </translation>
    </message>
    <message>
        <source>Always try to send message through Miasto Plusa SMS gateway - If you choose this option, module will each
time try to send message, even if you don&apos;t have any free SMSes to send on your account. Good choice for
people, that often send messages to other networks.

Never send messages to other networks that PlusGSM through Miasto Plusa gateway - If you choose this option,
module will alaways refuse to send message to network other than PlusGSM , so the next gateway in the
&quot;Gateways priority&quot; will be asked to send SMS. Choice good for people, that never use Miasto Plusa gateway
to send messages to nesworks other than PlusGSM using Miasto Plusa.</source>
        <translation type="obsolete">Zawsze próbuj wysłać wiadomość przez bramkę Miasta Plusa - Jeśli wybierzesz tą opcję, moduł zawsze będzie
próbował wysłać wiadomość, nawet gdy nie masz darmowych SMS-ów do innych sieci na koncie. Dobry wybór dla
osób które często wysyłają SMS-y do innych sieci.

Nigdy nie wysyłaj wiadomości do sieci innych niż PlusGSM przez bramkę Miasta Plusa - Jeśli wybierzesz tą
opcję, moduł zawsze będzie odmawiał wysłania wiadomości do sieci innej niż PlusGSM, więc następna bramka
w &quot;Priorytety bramek&quot; zostanie odpytana. Wybór dobry dla osób nie wysyłających SMS-ów do sieci innych niż
PlusGSM przez Miasto Plusa.</translation>
    </message>
    <message>
        <source>Confirmation type:</source>
        <translation type="obsolete">Rodzaj potwierdzenia:</translation>
    </message>
    <message>
        <source>Confirmation method
<byte value="x9"/>none - no confirmation
<byte value="x9"/>www - write receive confirmation on plus.pl
<byte value="x9"/>sms - send receive confirmation via SMS</source>
        <translation type="obsolete">Metoda potwierdzenia
<byte value="x9"/>none - brak potwierdzenia
<byte value="x9"/>www - potwierdzenie pojawi się w portalu plus.pl
<byte value="x9"/>sms - potwierdzenie zostanie dostarczone w postaci SMS&apos;a</translation>
    </message>
</context>
<context>
    <name>SendThread</name>
    <message>
        <source>Problem with connection to www.miastoplusa.pl!</source>
        <translation type="obsolete">Problem z połączeniem do www.miastoplusa.pl!</translation>
    </message>
    <message>
        <location filename="../sendthread.cpp" line="357"/>
        <source>Certificate verification error!!! Someone is propabely messing up with you!!! Aborting.</source>
        <translation>Błąd weryfikacji certyfikatu!!! Anuluję.</translation>
    </message>
    <message>
        <location filename="../sendthread.cpp" line="359"/>
        <source>libcurl said:</source>
        <translation>libcurl zwrócił:</translation>
    </message>
    <message>
        <location filename="../sendthread.cpp" line="359"/>
        <source>Some connection error has occured!</source>
        <translation>Wystąpił nieznany błąd połączenia!</translation>
    </message>
    <message>
        <location filename="../sendthread.cpp" line="363"/>
        <source>Login failed! Propabely wrong login name or password. SMS was not sent.</source>
        <translation>Logowanie nieudane! Prawdopodobie zła nazwa użytkownika lub hasło. SMS nie został wysłany.</translation>
    </message>
    <message>
        <source>Sending SMS failed! Gateway limit of sending 10 messages to the same number reached.</source>
        <translation type="obsolete">Wysyłanie SMS-a nie powiodło się! Osiągnięto limit 10-ciu wiadomości pod ten sam nr. w ciągu doby.</translation>
    </message>
    <message>
        <source>Sending SMS failed! Gateway limit of sending 10 messages to other networks reached.</source>
        <translation type="obsolete">Wysyłanie SMS-a nie powiodło się! Osiągnięto limit 10-ciu wiadomości do innych sieci w ciągu 10 dni.</translation>
    </message>
    <message>
        <location filename="../sendthread.cpp" line="369"/>
        <source>Unknown error has occured while trying to send an SMS.</source>
        <translation>Wystąpił nieznany błąd podczas wysyłąnia SMS-a.</translation>
    </message>
    <message>
        <source>SMSes sent to Plus during last 10 days: </source>
        <translation type="obsolete">SMS-y wysłane do Plusa w ciągu 10-ciu ostatnich dni: </translation>
    </message>
    <message>
        <source>SMSes sent to other networks during last 10 days: </source>
        <translation type="obsolete">SMS-y wysłane do innych sieci w ciągu 10-ciu ostatnich dni: </translation>
    </message>
    <message>
        <source>Sending SMS failed! Gateway limit of sending 30 messages to other networks reached.</source>
        <translation type="obsolete">Wysyłanie SMS-a nie powiodło się! Osiągnięto limit 30-ciu wiadomości do innych sieci w ciągu 10 dni.</translation>
    </message>
    <message>
        <source>Sending SMS failed! Account temporally blocked due to one of a gateway limits!</source>
        <translation type="obsolete">Wysyłanie SMS-a nie powiodło się! Konto czasowo zablokowane z powodu przekroczenia limitu bramki!</translation>
    </message>
    <message>
        <source>Account will be unblocked: </source>
        <translation type="obsolete">Konto zostanie odblokowane: </translation>
    </message>
    <message>
        <location filename="../sendthread.cpp" line="376"/>
        <source>SMSes to other networks left on an account: </source>
        <translation>Ilość SMS-ów do innych sieci pozostałych na koncie: </translation>
    </message>
    <message>
        <location filename="../sendthread.cpp" line="365"/>
        <source>You have no free messages to networks other than PlusGSM left.</source>
        <translation>Nie masz już żadnych SMS-ów do innych sieci na koncie.</translation>
    </message>
    <message>
        <location filename="../sendthread.cpp" line="367"/>
        <source>Spam protection: SMS was not sent.</source>
        <translation>Ochrona przeciwko spamowi: SMS nie został wysłany.</translation>
    </message>
    <message>
        <location filename="../sendthread.cpp" line="354"/>
        <source>Problem with connection to www.plus_pl.pl!</source>
        <translation>Błąd połączenia z www.plus_pl.pl!</translation>
    </message>
</context>
<context>
    <name>SmsMiastoplusaGateway</name>
    <message>
        <source>You mast specify your login name and password to Miasto Plusa in SMS tab in configure dialog!</source>
        <translation type="obsolete">Musisz podać nazwę użytkownika i hasło do konta w Mieście Plusa w oknie konfiguracyjnym!</translation>
    </message>
</context>
<context>
    <name>SmsMiastoplusaGatewaySlots</name>
    <message>
        <source>Always try to send message through Miasto Plusa SMS gateway</source>
        <translation type="obsolete">Zawsze próbuj wysłać wiadomość przez bramkę Miasta Plusa</translation>
    </message>
    <message>
        <source>Never send messages to other networks that PlusGSM through Miasto Plusa gateway</source>
        <translation type="obsolete">Nigdy nie wysyłaj wiadomości do sieci innych niż PlusGSM przez bramkę Miasta Plusa</translation>
    </message>
    <message>
        <source>Always</source>
        <translation type="obsolete">Zawsze</translation>
    </message>
    <message>
        <source>Never</source>
        <translation type="obsolete">Nigdy</translation>
    </message>
    <message>
        <source>none</source>
        <translation type="obsolete">none</translation>
    </message>
    <message>
        <source>www</source>
        <translation type="obsolete">www</translation>
    </message>
    <message>
        <source>sms</source>
        <translation type="obsolete">sms</translation>
    </message>
</context>
<context>
    <name>SmsPlusPlGateway</name>
    <message>
        <location filename="../sms_plus_pl_gateway.cpp" line="45"/>
        <source>You mast specify your login name and password to Miasto Plusa in SMS tab in configure dialog!</source>
        <translation type="obsolete">Musisz podać nazwę użytkownika i hasło do konta w Mieście Plusa w oknie konfiguracyjnym!</translation>
    </message>
    <message>
        <location filename="../sms_plus_pl_gateway.cpp" line="45"/>
        <source>You must specify your login and password to Plus.pl in SMS tab in configure dialog!</source>
        <translation>Musisz podać nazwę użytkownika i hasło do konta Plus.pl w oknie konfiguracyjnym!</translation>
    </message>
</context>
</TS>
