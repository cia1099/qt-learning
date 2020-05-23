> 高手都用Qt進行Android專案 編成 ISBN:9789863793083
## Linguist
Qt提供用於應用軟件軟體國際化的工具，可以把待當地語系化的文字轉換成任一種語言，編譯成Qt應用可以使用的qm檔案。[p.2-33, p.6-64]

## qmake
產生專案所需的makefile
範例：
```qmake
greaterThan(QT_MAJOR_VERSION, 4) #cmake_minimum_required(VERSION 3.10.0)
QT += widgets #find_package(Qt5 COMPONENTS Widgets REQUIRED)
TARGET = HelloWorld #目的檔案名稱 add_executable(HelloWorld main.cpp)
### TARGET EXAMPLE
CONFIG(debug, debug|release){
    TARGET = HelloWorld_d
    message("debug build")
}else{
    TARGET = HelloWorld
    message("release build")
}
###
SOURCE += main.cpp #add_executable(HelloWorld main.cpp) 加入建置檔案
INCLUDEPATH += d:/opencv/include \ #include_directories() 加入表頭檔路徑
                d:/google/protobuf/include
LIBS += -Ld:/opencv/lib -lopencv_world #link_directories(d:opencv/lib), link_libraries(opencv_world)
DEFIBES += _WIN32 #add_definitions(_WIN32)
TEMPLATE = app #重要；看下面附註
```
* TEMPLATE變數用來設定專案的建置類型，目前qmake支援下列範本：
■ app，預設的範本，qmake會產生一個建置用的makefile
■ lib，qmake會產生一個makefile用來建置動態或靜態庫
■ subdirs，子目錄專案，qmake會根據SUBDIRS變數產生一個makefile，該makefile套件含建置子目錄的邏輯，子目錄內必須有.pro檔案存在
■ vcapp，產生一個適用於Visual Studio的專案(.vcxproj)可以建置.exe檔案
■ vclib，適用於VS的專案(.vcxproj)來建置動態或靜態庫
■ vcsubdir，產生適用於VS的解決方案(.sln)，裡面包含每個子目錄對應的專案檔案(.vcxproj)。可以使用VS開啟解決方案並建置每個子專案
* qmake對變數的引用(dereference)語法為`$${MY_VAR}`
* 每一個使用Qt GUI的應用程式，main()函數的第一行一般都要加上`QApplication`物件；每一個應用，只可以有一個QApplication實例，在任何使用GUI資源前都要先實體化該物件。[p.5-2]
* `return QApplication::exec();`使得應用程式進入Qt的事件循環中，直到使用者透過界面互動選擇退出程式。[p.5-3]
* 要在執行緒中使用QTimer, QTcpSocket, QNetworkAccessManager等相依事件循環的類別庫，必須顯示地呼叫`exec()`開啟執行緒自己的事件循環。自己所在的執行緒並呼叫`QThread::currentThread()->exit()`方法退出循環事件。[p.6-30]
* 由於很多Qt類別的建置函數都有一個父物件參數，在執行緒物件的`run()`方法裡建立新物件時，經常把<font color=000ff>this</font>指標傳遞給新建立的物件作為新物件的父，這是錯誤的用法。[p.9-3]
## Android environment
* 安裝JDK，需要去Oracle下載，安裝完後再設定環境變數，可以用cmd命令`java -version`測試有沒有成功。[p. 3-6]
* Android SDK安裝，如果是用Jave開發才需下載ADT Bundle或Android Studio，否則單獨安裝SDK工具(Download the SDK Tools for Windows)即可。安裝完後將adb.exe所在路徑加入系統變量，用cmd命令`adb version`測試。
* Android NDK安裝
* Apache Ant安裝，Ant是用來自動編譯Android專案的工具，安裝只需把壓縮檔解壓至某個位置即可，建議和Android SDK放在同一個目錄下，並將bin目錄添加至環境變數，用cmd命令`ant`測試。

環境都安裝好之後，開啟qt creator選項中的"Android"設定SDK與NDK的路徑，基本上會自動檢測到。然後再選項中的"建置和執行"查看分頁"Kits"可以看到『Android for armeabi』等套件，那摸Creator關於Android的環境設定已經完成。

## 日誌系統
我們可以透過呼叫qInstallMessageHandler(void (*f)())函數來註冊自己的訊息處理器((*f)()函數指針)，取代Qt架構提供的預設訊息處理器。[p.3-40]
atexit(void (*f)())可用來註冊關閉紀錄檔，方便文件管理。[p.3-41]
用QFile實現簡單的日誌類型的範例[p.11-2-p.11-6]

## Property
想要在一個類別中使用`Q_PROPERTY()`mecro宣告一個屬性，該類別必須是QObject的後裔；同時也必須在類別本體的開始引用Q_OBJECT macro。[p.6-33, 6-34]
* READ、WRITE、RESET等標記指定的函數，可以是繼承來的，還可以是虛函數。如果是多重繼承，則必須來自第一個繼承類別。不管怎麼說，都<font color=ff00>必須是你類別中存在的函數</font>，Q_PROPERTY()不會自動為你產生這些函數。
* 屬性類型可以是QVariant支援的任意型別。也可以是使用者定義的類型。
* <font color=ff00>Q_PROPERTY()的參數中不能包含逗點","，逗點會被認為是macro引數分隔符號。</font>所以一個屬性的類型是QMap的話，就不能將QMap<int,QString>直接放到macro內。
e.g.
```cpp
/*宣告了類型為bool的visible屬性，可以透過isVisible()函數存取
透過setVisible(bool)函數改變它,這個屬性在Qt Designer中不可見*/
Q_PROPERTY(bool visible READ isVisible WRITE setVisible DESIGNABLE false)
/*使用setWindowIcon(QIcon)函數改變windowIcon
當windowIcon改變時會發射windowIconChanged訊號
在Qt Designer中是否可用則相依於該類別的isWindow()成員函數的回傳值*/
Q_PROPERTY(QIcon windowIcon READ windowIcon WRITE setWindowIcon NOTIFY windowIconChanged DESIGNABLE isWindow) 
```
e.g. 屬性可以透過QObject類方法property(), setProperty()來間接調用，而且以setProperty()屬性名稱間接的方式調用時，當屬性名稱不存在，會建立一個以你提供的名稱命名的屬性並將其值設定為你的傳入值，之後又可以透過property()方法讀取這個屬性的值。
```cpp
QPushButton *button = new QPushButton(this);
QObject *object = button;
button->setFlag(true);
object->setProperty("flag",true);
```
e.g. 利用QMetaObject類來遍歷一個物件的所有屬性
```cpp
QObject *object = ...
const QMetaObject *mobj = object->metaObject();
int count = mobj->propertyCount();
for(int i=0; i!=count; ++i){
    QMetaProperty m_property = mobj->property(i);
    const char* pname = m_property.name();
    QVariant value = object->property(name);
} 
```
* QObject類別有一個屬性叫作objectName，是這麼定義的：[p.6-54]
```cpp
Q_PROPERTY(QString objectName READ objectName WRITE setObjectName NOTIFY objectNameChanged)
```
當透過setObjectName()設定物件名稱，會發出訊號objectNameChanged()。用這個設定好的名稱，可以用來搜索物件，例如：[p.6-55]
```cpp
//尋找parentWidget的名為"button1"的類型為QPushButton的子代
QPushButton *button = parentWidget->findChild<QPushButton*>("button1");
//傳回parentWidget的類型為QListWidget的子代
QListWidget *list = parentWidget->findChild<QListWidget*>();
//尋找parentWidget的子代中名稱為"button1"或"Button1"，類型為QPushButton的物件
QPushButton* btn = parentWidget->findChild<QPushButton*>(QRegExp("[Bb]utton1"));
```
### QMessageBox
訊息方塊是個模態(出現時後面的視窗會無法操作)對話方塊，因為用exec()方法顯示訊息方塊。[p.5-20]
Qt把需要用到訊息方塊的幾種場景都預設了，提供了一組靜態方法：about(), information(), warning(), critical(), question()以便我們快速建立訊息方塊。如果只使用預設按鈕，那麼exec()或information()等靜態方法的int回傳值可以告訴你使用者觸發了哪個角色(Role)，可以根據這個決定如何回應使用者。
```cpp
//e.g.[p.5-12]
void Widget::onSubmit(){
    QString content;
    for(int i=0;i!=m_nQuestionCount;++i){
        content += QString("problem%1: %2Score\n").arg(i+1).arg(m_score[i]->currentText());
    }
    QMessageBox *msg = new QMessageBox(QMesssageBox::Information, "Result",
        content, QMessageBox::Ok, this);
        msg->exec(); //將循環事件鎖在msg對話匡，以至於後面的窗體無法操作
        //msg->information();// equivalent?
}
```
### 事件篩檢
作為事件篩檢的類別，必須是QObject的衍生類，透過重定義QObject::evenFilter()方法來接收事件做選擇性處理。eventFilter()原形如下：[p.6-40]
```cpp
bool QObject::eventFilter(QObject* watched, QEvent* event);
```
輸入參數event原來是發給watched物件，但我們安裝了篩選物件後，事件的執行會先攔截並執行於重定義的eventFilter()方法︰實現了eventFilter()後，還需要把重定義好的物件安裝至watched物件上，利用watched物件的成員函式`QObject::installEventFilter(QObject* filterObj)`安裝；通常傳入的參數**filterObj**為watched物件的**parent**。[p.6-41, 6-42]

### 動態類型轉換
當在類別宣告中使用了Q_OBJECT macro後，就可以使用Qt的動態類型轉換。[p.6-63, 6-64]
e.g.
```cpp
QObject *obj = new QPushButton(widget);
QAbstractButton* btn = qobject_cast<QAbstractButton*>(obj); //btn == (QAbstractButton*)obj
QRadioButton* rdo = qobject_cast<QRadioButton*>(obj); //rdo == 0
```
QObject有個方法`inherits(const char*)`，可以判斷一個QObject(或其衍生類)的實例是否從指定名稱的類別繼承而來。[p.6-63]e.g.
```cpp
QPushButton *btn = new QPushButton(widget);
btn->inherits("QObject"); //true
btn->inherits("QWidget"); //true
btn->inherits("QLayoutItem"); //false
```

### QTableWidget
* 其實QTavleWidget的儲存格可以接受任意類型的QWidget──呼叫setCellWidget()方法來設定。setCellWidget()的第三個參數**接受QWidget物件指標**。QTableWidgetItem其實只是個輔助類別，提供設計好的界面讓我們管理文字、圖示、對齊、字形等。[p.8-24]
* QTableView的`setSpan(int row, int col, int rowSpanCount, int columnSpanCount)`方法可以**合併儲存格**。前兩個參數指定要擴充的儲存格位置，後兩格參數指定要擴充的列數和行數。如setSpan(0,0,2,2)則會使得表格左上角第一個儲存格合併包圍它那三個儲存格，最後出現一個2×2大小的儲存格。[p.8-26]

## MultiThread
* QThread

`QThread::run()`特指執行緒的主函式，即在新執行緒中執行直到執行緒結束的函數，類似main()函數。當呼叫`QThread::start()`方法後，把QThread物件本身的指標儲存在執行緒本機(TLS)或某個神秘的地方，當執行緒的主函數結束後再把這個指標取出來使用，發射finished()訊號通知關注執行緒物件的朋友們。[p.9-3]

執行緒物件的生命週期比執行緒的生命週期長，執行緒物件先出生，執行緒後出生，執行緒先死亡，執行緒物件後死亡。
執行緒主要步驟：繼承QThread→重新定義run()函數→建置執行緒物件→呼叫start()
在Qt中，每個執行緒都可以有自己的事件循環，透過QThread::exec()啟動執行緒的事件循環。[p.9-5]

QThread衍生類別的實例，即執行緒物件，屬於建立它的執行緒。[p.9-6]

一定要注意，應該讓工作執行緒中的工作者(在run()函數中分配的完成實際工作的物件)處理事件，而非QThread衍生類別的實例(即執行緒物件)──因為執行緒物件與主執行緒中的物件同屬一個執行緒。要向一個工作執行緒中的物件投遞事件，可以使用QCoreApplication::postEvent()函數，它是執行緒安全的。[p.9-7]

在非主執行緒中的工作者(新建的heap實體)，可以交給QScopedPointer管理，這樣我們就不用操心工作者的生死了。在工作執行緒中動態分配的物件，最好在工作執行緒中銷毀。[p.9-12]

**※QThread注意事項**：[p.9-16]
1. 父子物件必須是處在同一個執行緒中，QThread物件不能是它啟動的那個執行緒中建立的物件的父。父物件在解構時會刪除子物件，這種行為的後果在跨緒程時事不可預知的，因為你刪除的物件可能正在被別的執行緒使用。
2. 相依事件循環的物件只能在同一個執行緒中使用，不能在執行緒A中建立，而在執行緒B、C中使用，又期待它的事件處理函數會在B、C中被呼叫；如果真的想把某個物件放在特定的執行緒中使用，可以呼叫`moveToThread()`方法。
3. 在工作執行緒中建立的物件必須在執行緒物件被銷毀之前刪除。一個辦法是在run()函數的stack上建立工作者物件，由工作者物件擔任所有其他物件的父，這樣工作者在run()結束時自動被銷毀，其子代們也會一同銷毀；另一個辦法是在stack上建立一個QObject Clearup Handler或QScopedPointer來管理從heap上分配的物件，依靠這些在run()結束時會自動解構的智慧指標來刪除其他物件。
4. 盡量不要在stack上建立QThread衍生類別實例，然後啟動執行緒，因為如果你的執行緒不能在擁有執行緒物件的函數結束前結束，就會帶來災難。
5. 盡量和諧地結束一個執行緒，然後再銷毀執行緒物件。例如你可以呼叫QThread::quit()通知一個執行緒準備結束，然後呼叫QThread::wait()等一下。
---
* QThreadPool

每個Qt應用都有一個全域的執行緒池，可以透過QThreadPool::globalInstance()存取。當你拿到執行緒池實例後，就可以向它投遞工作了。[p.9-17]
e.g. QThreadPool的start()方法可以接受一個QRunnable衍生類別的實例，然後呼叫約定的run()方法執行它不了解也不關心的工作(這個工作如何完成只有傳送者知道)
```cpp
class HelloTask : public QRunnable{
    void run(){
        qDebug << "Hello World from thread" << QThread::currentThread();
    }
};
...
HelloTask *hello = new HelloTask();
// QThreadPool takes ownership and deletes 'hello' automatically
QThreadPool::globalInstance()->start(hello);
```
■ QThreadPool::setExpiryTimeout()，可以設定空間執行緒的存活週期(如果不設定，過30秒就退了)
■ QThreadPool::setMaxThreadCount()，可以設定執行緒池內的最大執行緒個數
■ QThreadPool::reserveThread()，可以超越限制預留一個執行緒出來
■ QThreadPool::clear()，可以清除還未執行的工作
■ QThreadPool::waitForDone()，可以等待所有執行緒退出

---
* QtConcurrent更高階的多執行緒API[p.9-18]
使用時搭配QFutureWatcher類別，該類可以用來追蹤一個QFuture物件，查看工作是否完成。
e.g.
```cpp
// example at 9.7 [p.9-19]
QImage loadImage(const QString& uri){
    QImage img(uri);
    return img;
}

Widget::Widget(QWidget* parent):QWidget(parent), m_watcher(this){
    /* 用QFutureWatcher的信號用以追蹤非同步工作的QFuture，有許多信號可以追蹤各種狀態
    ，利用不同狀態連接設計相對應反應的槽函數 */
    connect(&m_watcher, SIGNAL(finished()), this, SLOT(onFinished()));
    //設定一個為時5秒的單次觸發計時器，以便觸發onLoad()槽來載入圖片
    QTimer::singleShot(5000, this, SLOT(onLoad()));
}

void Widget::onLoad(){
    QString uri(":/airplay.gif");
    /* 使用QtConcurrent 命名空間內的run()方法傳送了一個非同步工作，
    指定執行工作的函數是loadImage()，函數需要的參數緊接著函數名稱一次傳入。
    run()函數的返回值是QFuture<T>類型的物件，它所使用的模板參數類型和
    run()方法要執行的工作函數的傳回類型相對應。即 T == QImage */
    QFuture<QImage> future = QtConcurrent::run(loadImage, uri);
    m_watcher.setFuture(future); //QFutureWatcher<QImage> m_watcher
}

void Widget::onFinish(){
    /* 透過QFutureWatcher物件取得目前的QFuture物件，
        然後呼叫QFuture::result()獲得非同步工作的結果 */
    m_image = m_watcher.future().result();
    if(!m_image.isNull())
        this->repaint();
}
```

## 網路程式設計 [p.10-5]
1. **QNetworkAccessManager**是中心，它提供了發送請求的各種API以及網路設定相關的方法。
2. **QNetworkRequest**代表一個網路請求，包含URL、HTTP存取用的UserAgent、使用者名稱、密碼等都是透過它設定。
3. **QNetworkReply**是QNetworkAccessManager根據你傳送的QNetworkRequest建置的回應物件，一旦請求發出，你就可以透過它獲取網路操作過程中的各種狀態。
4. **QUrl**是建置請求(QNetworkRequest)時常用的類別，它可以編譯碼URL，分析URL中的主機、路徑、檔案、使用者名稱、密碼等資訊。
---
* 歸納使用QNetworkAccessManager進行http程式設計的基本步驟：[p.10-14]
1. 根據URL產生QUrl物件，然後根據QUrl建立一個網路請求QNetworkRequest，必要時可以設定一些header。
2. 呼叫QNetworkAccessManager的get()、post()等方法。
3. 使用QNetworkAccessManager傳回的QNetworkReply實例來追蹤回應的各種狀態回饋。
※注意事項：
1. 開發者必須自己選擇合適的時機釋放QNetworkAccessManager傳回的QNetworkReply物件。
2. 請不要在回應QNetworkReply物件的訊號時直接刪除QNetworkReply物件。應該使用其deleteLater()方法延遲銷毀它。
3. 對於重新導向訊息，可以透過QNetworkReply::header (QNetworkRequest::LocationHeader)取得定向到的新位址，然後針對新位址重走長征路。
---
* 使用QTcpSever進行伺服器軟體開發的一般步驟：[p.10-25]
1. 從QTcpServer繼承來實現代表服務的類別(也可以透過聚合方式把QTcpServer的物件放在服務類別中)。
2. 呼叫QTcpServer::listen()<font color=ff00>"監聽"服務通訊埠</font>，等待用戶端連接。
3. 連接newConnection()訊號或重新定義incomingConnection()方法，使用QTcpSocket與傳入連接互動。

* 使用QTcpSocket客戶端程式設計的基本步驟：[p.10-29]
1. 建置QTcpSocket物件
2. 呼叫connectToHost()連接伺服器
3. 連接connect()、readyRead()、error()等訊號，處理資料和錯誤
4. 定義業務相關的邏輯，根據網路互動結果來觸發

※注意事項：
我們先判斷socket上的資料是否足夠4個byte(協議約定)，如果不夠則等待下一次readyRead()訊號在處理。因為以TCP為基礎的連接，資料是沒有邊界的，服務端一次寫入100個byte，用戶端可能一次全收到也可能分很多次收到；而服務端每次寫入100個byte間歇性寫3次，用戶端卻又有可能一次就全部讀出來了，這是TCP通訊中所謂的「黏包」概念。[p.10-27]
讀取4個byte，使用ntohl()函數<font color=ff00ff>轉換網路byte組序為本機byte組序</font>(使用byte組序轉換函數ntohl、htonl等，Windows平台需要包含winsock2.h表頭檔，Android平台需要包含netinet/in.h)。[p.10-28]

---
* 使用QUdpSocket實現伺服器程式的基本步驟：[p.10-31]
1. 從QUdpSocket繼承(或聚合一個QUdpSocket物件)來實現服務類別
2. 呼叫QUdpSocket::bind()<font color=ff00>"綁定"到一個通訊埠</font>等待資料封包的到來
3. 連接readyRead()訊號處理到達的資料封包
* 使用QUdpSocket進行客戶端程式設計的一般步驟：[p.10-34]
1. 從QUdpSocket繼承(或聚合一個QUdpSocket物件)來實現用戶端類別
2. 呼叫QUdpSocket::bind()<font color=ff00>"綁定"到一個通訊埠</font>以便接收服務端的資料封包
3. 呼叫writeDatagram()發送資料
4. 連接readyRead()訊號到一個槽，在槽內處理到達的資料封包

※比較UDP與TCP差異：[p.10-31]
1. UDP啟動server時使用`bind()`，而TCP使用`listen()`
2. UDP透過連接readyRead()訊號處理用戶端的查詢請求，透過呼叫`readDatagram()`來取得用戶端的位址和通訊埠，然後以此位址和通訊埠為參數呼叫`writeDatagram()`寫回資料；而TCP連接newConnection()訊號或重新定義`incomingConnection()`方法，<font color=ff00ff>獲得一個已建立的socket連接，透過此連接來發送資料</font>。
3. UDP與TCP在客戶端相比，少了`connectToServer()`方法，這正是無連接和連線導向的差別，UDP是不需要連線的。[p.10-32]

※總結：
UDP服務端和客戶端綁在同一個通訊埠上，彼此都不關心資料傳送次序的問題，只管發送自己的資料，讀取也是各讀各的在埠上的資料，彼此不關心對方是否收到也不接收對方的封包。

### QDataStream
Java中有專門的序列化類別庫Serializable, ObjectOutputStream, ObjectInputStream等用於物件的序列化和反序列化。C++中沒有這麼專業的東西，通常要想把程式中的資料結構儲存到檔案並期望能夠從檔案還原需要付出很多努力，不過QDataStream可以省去從頭造輪子的負擔。QDataStream類別用來讀寫二進位資料，可以用來做資料的序列化。[p.11-8]

### XML
XML只可延伸標記語言，它被設計用來傳輸和儲存資料。[p.12-2]
XML的讀寫可以使用DOM、SAX和StAX三種方式；Qt官方建議使用StAX模式來處理XML檔案，因為QtXML模塊已經不在其活耀維護列表中了。[p.12-11]
DOM(文件物件模型)方式會在記憶體中產生一顆樹，它需要完整掃描分析整個XML檔案，過程較慢，佔用記憶體多，但API最容易使用。當需要多次利用同一個XML的解析結果，或需要在DOM樹中頻繁無序地尋找時，DOM是最合適的。
StAX(Stream API for XML)在Qt中透過流的方法，具有比DOM還要好的效能，所佔用的記憶體也很小。

```xml
<?xml version="1.0" encoding="utf-8"?>
<peoples>
    <people name="John">
        <mobile>13302901234</mobile>
    </people>
    <people name="Mary">
        <mobile>13502901234</mobile>
    </people>
</peoples>
```

e.g. QXmlStreamReader
```cpp
struct Contact{
    QString m_name;
    QString m_mobile;
};

bool parseXml(const QString &fileName, QList<Contact*> &contacts){
    QXmlStreamReader reader;
    QFile file(fileName);
    file.open(QFile::ReadOnly);
    reader.setDevice(&file);
    QStringRef elementName;
    Contact* contact = null_ptr;
    while(!reader.atEnd()){
        reader.readNext();
        if(reader.isStartElement()){
            contact = new Contact;
            Contact->m_name = reader.attributes().value("name").toString();
        }
        else if(elementName == "mobile"){
            contact->m_mobile = reader.readElementText();
            contacts.append(contact);
        }
    }
    file.close();
    return (!reader.hasError() && file.error() == QFile::NoError);
}
```
e.g. QXmlStreamWriter
```cpp
bool writeXmlByStreamWriter(const QString& fileName){
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);

    writer.writeStartDocument("1.0");
    writer.writeStartElement("peoples");
    writer.writeStartElement("people");
    writer.writeAttribute("name", "Mary");
    writer.writeTextElement("mobile", "13502901234");
    writer.writeEndElement();
    writer.writeEndElement();
    writer.writeEndElement();

    file.close();
    return true;
}
```

### Qt中的Json類庫
Qt可以解析文字形式的Json資料，使用`QJsonDocument::fromJson()`方法即可；也可以解析編譯為二進位的Json資料，使用`fromBinaryData()`或`fromRawData()`。對應地，`toJson()`和`toBinaryData()`則可以反向轉換。一旦fromJson()方法傳回，Json資料就都被解析為Qt定義的各種Json物件，如**QJsonObject, QJsonArray, QJsonValue**等，可以使用這些類別的方法來查詢你有興趣的內容。[p.16-73]

## Qt樣式表
進入Qt幫手的索引模式，以"Qt Style Sheets Reference"為關鍵字檢索。Qt樣式表是一種快速改變Qt程式UI風格的方法，它快速地參考和參考了HTML層疊樣式表(CSS，Cascading Style Sheets)的語法和思想。[p.16-82]
你可以使用QApplication::setStyleSheet()來給整個應用設定全域的樣式表，也可以使用QWidget::setStyleSheet()給某個特定的widget及其子代們設定樣式表。如果你設定幾個不同層次的樣式表，Qt會從那些樣式表中繼承有效的部份，這就是所謂的樣式表串聯。[p.16-83]

e.g. 開發者必須透過字串來設定樣式表
```cpp
QWidget w;
w.setStyleSheet("* {background-color: #050306;}");
// 也可以將樣式儲存成檔案，整合到Qt資源檔系統中
QFile file(":/qss/yourstylesheet.qss");
file.open(QFile::ReadOnly);
app.setStyleSheet(file.readAll());
```
設定在窗體上的樣式表會影響它的子代們的外觀。
「類別名稱 # 物件名稱」是限定樣式規則的作用範圍是特定名稱的物件，如`QPushButton# okBtn`表示樣式規則比對所有類行為QPushButton名字為okBtn的物件。