#include <cstdarg>
#include <cstdio>
#include <atomic>
#include <iostream>
#include <future>
#include "messageQueue.h"
#include "threadPool.h"
#include "AuditLog.h"
#define ASSERT assert
#define ISNULL(n) ((n==NULL)?(0):(1))
char datafile [ 255 ];
char outputhead [ 1024 ];
char outputtail [ 1024 ];
std::atomic<int> msgno;
int inputtkts;
AuditLog AUDLOG;
struct MsgQueue
{
	long tmsgtype;
	char bmsg [ 5120 ];
}mk;


    static int CloseFile ( FILE *p )
    {
        return fclose ( p );
    }

        static FILE *OpenFile ( const char *path , const char *mode )
    {
        return fopen ( path , mode );
    }



/**
* 类似strcat拼字符串
* Dbuf:原字符串(一行票信息)
* Obuf:需要拼接的目标字符串
*/
int StrCat ( char *Dbuf , char *Obuf )
{
    char *p1 = Obuf; int i = strlen ( Dbuf ); char buf [ 13 ];
    if ( !ISNULL ( p1 ) )
    {
        AUDLOG.logfile<< "p1 is NULL" << endl;
        return i;
    }
    if ( i > 4566 )
	{
		AUDLOG.logfile << "NORMAL-input length > msg length\n" ;
		snprintf ( buf , 12 , "%s" , Obuf );
		AUDLOG.logfile << buf <<endl;
		return i;
	}

    while ( *p1 != '\0' )
    {
        Dbuf [ i++ ] = *p1++;
    }
    Dbuf [ i ] = '|';
    return i;
}

class tktprocessing {
    public:
    template<typename Msg>
    void analysisInputTkt(Msg mq) {
    char row [ 1024 ] , nextlineno [ 12 ] , curlineno [ 12 ];
    FILE *TP = OpenFile ( "G:\\CLION_PRAC\\opsa-newschedulengine\\20181024.332004" , "r" );
    if ( !ISNULL ( TP ) )
    {
        AUDLOG.logfile<< "open inputfile faild"<<endl;
        return ( void ) 0;
    }
    strcpy ( nextlineno , "00000000001" );
    memset ( curlineno , 0 , sizeof( curlineno ) );
    memset ( outputhead , 0 , sizeof( outputhead ) );
    memset ( outputtail , 0 , sizeof( outputtail ) );

    mk.tmsgtype = 3;

    while ( !feof ( TP ) )
    {
        memset ( row , 0 , sizeof( row ) );
        memset ( curlineno , 0 , sizeof( curlineno ) );
        fgets ( row , sizeof( row ) , TP );
        strncpy ( curlineno , row , 11 );
        if ( atoi ( curlineno ) == 0 )
        {
            strncpy ( outputhead , row , strlen ( row ) );
            continue;
        }
        if ( atoi ( curlineno ) > atoi ( nextlineno ) )
        {
            msgno += 1;
            if ( !mq->MsgSend(  mk ) )
            {
                AUDLOG.logfile<< "sendmsg faild errno=" << errno <<endl;
                std::cout<<errno<<std::endl;
            }
            memset ( nextlineno , 0 , sizeof( curlineno ) );
            strncpy ( nextlineno , curlineno , 11 );
            memset ( mk.bmsg , 0 , sizeof( mk.bmsg ) );
        }
        if ( strncmp ( row , "99999999999" , 11 ) == 0 )
        {
            strncpy ( outputtail , row , strlen ( row ) );
            break;
        }
        StrCat ( mk.bmsg , row );
    }
    inputtkts = msgno;
    CloseFile ( TP );
    AUDLOG.logfile << "inputfile tickets" << inputtkts <<endl;
    std::cout << "input tktnum: "<< inputtkts<<std::endl;

}

bool outputResult(std::shared_ptr<messageQueue<MsgQueue>> mq) {
    auto res = mq->MsgGet();
    if(res==nullptr) return false;
    AUDLOG.logfile<<res->bmsg<<endl;
    return true;
}

};




int main()
{
    AUDLOG.log_name = AUDLOG.GetSysTime();  //以程序执行时间命名log日志文件
	AUDLOG.logfile.open(AUDLOG.log_name, ios_base::out | ios_base::app); //打开文件，以便写入，并追加到文件尾
	//下面一段代码可用ASSERT代替
	ASSERT(AUDLOG.logfile.is_open()); //若无法打开log日志文件，则返回错误码并结束
    std::cout << "start to test message queue!"<<std::endl;
    std::shared_ptr<messageQueue<MsgQueue>> mq = std::make_shared<messageQueue<MsgQueue>>(mk.tmsgtype);
    tktprocessing *tktprocess{};
    auto s1 = std::async(&tktprocessing::analysisInputTkt<std::shared_ptr<messageQueue<MsgQueue>>>, tktprocess, mq);
    s1.get();
    threadPool pool;
    std::vector<std::future<bool>> results;
    for (int i = 0; i < msgno.load(); i++) {
        results.emplace_back(pool.submit(&tktprocessing::outputResult,tktprocess,mq));
        
    }
    int i=0;
    for (auto&& res : results) {
        std::cout << res.get()<<std::endl;
        i++;
        std::cout<<i<<std::endl;
    }
    AUDLOG.logfile.close();
    system("pause");
    return 0;
}
