#include<Servo.h> //Servo kütüphanesi dahil edildi.
#include<SoftwareSerial.h>  //SoftwareSerial kütüphanesi dahil edildi.

//Motorlar servo olarak tanımlandı
Servo sag_on_teker; 
Servo sag_arka_teker;
Servo sol_on_teker;
Servo sol_arka_teker;

//Motorların min-max PWM değerleri, ileri ve geri hız değerleri kod bloğu içerisinde değişmeyecek şekilde tanımlandı.
#define Max 870
#define Min 870
#define hiz 700
#define ghiz 300

String state;//Bluetooth üzerinde gelen mesajlar burada tutulur.
SoftwareSerial BT(18,19); //Arduino (RX,TX) - HC06 Bluetooth (TX,RX). Bluetooth haberleşmesi için pinler belirlendi.

//Uzaklık sensörlerinin pin değerleri ve maksimum uzaklık değeri kod bloğu içeerisinde değişmeyecek şekilde tanımlandı.
#define trigPin 7
#define echoPin 6
#define Max_Uzaklik 5 //5 cm

//long 64 bit uzunluktadır. Mesafe hesabı için long ile tanımlama yapıldı.
long uzaklik;
long sure;

//Tanımlamalar yapılır.
void setup() {  

  Serial.begin(9600); //Saniyede 9600 bit hızında veri gönderir. Seri port ekranı için başlatıldı.
  Serial1.begin(9600);  //Serial1, bluetooth için başlatıldı.

  pinMode(trigPin, OUTPUT); //Uzaklık sensörünün ses dalgası gönderen kısmıdır. Pin ve ses dalgası gönderdiği için çıkış olarak ayarlandı.
  pinMode(echoPin, INPUT);  //Sensörün ses dalgasını alan kısmıdır. Pin ve ses dalgası aldığı için giriş olarak ayarlandı.

 // pinMode(13, OUTPUT); Bluetooth haberleşmesini test etmek için konulan led

/*Motorların kontrol girişine gelen sinyal uzunluğuna göre açı değerini değiştirir. Belirli sinyal aralığında çalışır.
  Kullanılan motor olan için bu değer (870,870)
*/
  sag_on_teker.attach(8,Min,Max); //Sağ ön teker motor pini PWM değerleriyle ayarlandı
  sag_arka_teker.attach(9,Min,Max); //Sağ arka teker motor pini
  sol_on_teker.attach(10,Min,Max);  //Sol ön teker motor pini
  sol_arka_teker.attach(11,Min,Max);  //Sol arka teker motor pini

/*Kullanılan ESC'lerin, her başlatıldığında kalibrasyonu yapılması gerekmektedir.
  Kullanılan motorlar için bu değer 500'dür.
  ESC'lerin en az 3 saniyede kalibrasyonu tamamlanmaktadır.
*/
  sag_on_teker.write(hizFonk(500)); // ESC kalibrasyonu
  sag_arka_teker.write(hizFonk(500)); // ESC kalibrasyonu
  sol_on_teker.write(hizFonk(500)); // ESC kalibrasyonu
  sol_arka_teker.write(hizFonk(500)); // ESC kalibrasyonu 
  delay(5000);  


}

void loop() { //İçerisindeki kodlar güç kesilinceye kadar çalışır

  while(Serial1.available()){ // Serial1 (Bluetooth) veri alışverişini kontrol eder
    delay(10);
//Veriler telefon-tablet uygulamasından String olarak gönderilir.
    String c = Serial1.readString();  //Bluetooth'tan gelen verileri okur ve değişkene atar.
//  Serial.write((Serial1.read())); //Bluetooth'tan veri geliyor mu kontrolü
    state += c; //Değişkene atanan değerler state değişkeninde birleştirlir
    Serial.println(state);  //Seri port ekranından Bluetooth'tan gelen veriler gösterilir
    }

    if(state.length()>0){ //Veri gelirse aşağıdaki işlemleri yapacaktır.

/*  Telefon-tablet uygulamasından gelen veriler kontrol bloklarında denetlenerek aracın istenilen hareketi yapması sağlanır.
    Mesafe sönserlerinden gelen bilgiyi okur ve bir yere çarpmamak için durur.
*/
      if(state == "sola"){
         solaDon();
         mesafeKontrol();
        }

      else if(state == "ileri"){
          ileriGit();
          mesafeKontrol();
        }

      else if(state == "saga"){
          sagaDon();
          mesafeKontrol();
      }
      else if(state == "sol"){
          solaGit();
          mesafeKontrol();
        }
      
      else if(state == "dur"){
          dur();
        }
      
      else if(state == "sag"){
          sagaGit();
          mesafeKontrol();
        }
      
      else if(state == "solageri"){
          solaGeriGit();
          mesafeKontrol();
        }
      
      else if(state == "geri"){
          geriGit();
          mesafeKontrol();
        }
      
      else if(state == "sagageri"){
          sagaGeriGit();
          mesafeKontrol();
        }
        
       else{  //Komut gelmezse durur.
          dur();
        }

        state = ""; //Bluetooth'tan gelen veri sıfırlanır ve döngünün başına döner.    
      } 
}

//Hız fonksiyonu

int hizFonk(int value){ //Belirli değer aralığında olan tam sayı değerleri değiştirilecek
  return map(value, 0, 1000, 55, 150);  //map(değişken, min mevcut değer, max mevcut değer, yeni min değer, yeni max değer)
}

//Mesafe kontrolü
void mesafeKontrol(){
  
  digitalWrite(trigPin, LOW); //trigPin'i başlat.
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);  //Ses dalgası gönder.
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);  //Yeni ses dalgası üretebilmesi için LOW değerinde

  sure = pulseIn(echoPin, HIGH);  //Gönderilen ses dalgasının geri dönme süresini hesaplar.
  uzaklik = sure/29.1/2;  //Ölçülen süre uzaklığa çevrilir.

  if(uzaklik < Max_Uzaklik){
/*    //Bluetooth ve mesafe sensörü denemesi için led denemesi
      digitalWrite(13,HIGH);  
      delay(1000);
      digitalWrite(13,LOW);
*/  
      dur();  //Mesafe belirtilen max uzaklıktan küçükse dur fonksiyonu çağırılır.
    }
  
  }

//Hareket Fonksiyonları
void ileriGit(){
  
  sag_on_teker.write(hizFonk(hiz));
  sag_arka_teker.write(hizFonk(hiz));
  sol_on_teker.write(hizFonk(hiz));
  sol_arka_teker.write(hizFonk(hiz));
  
  }

void geriGit(){
  
  sag_on_teker.write(hizFonk(ghiz));
  sag_arka_teker.write(hizFonk(ghiz));
  sol_on_teker.write(hizFonk(ghiz));
  sol_arka_teker.write(hizFonk(ghiz));
  }

void sagaGit(){
  
  sag_on_teker.write(hizFonk(hiz));
  sag_arka_teker.write(hizFonk(ghiz));
  sol_on_teker.write(hizFonk(hiz));
  sol_arka_teker.write(hizFonk(ghiz));
  
  }


void solaGit(){

  sag_on_teker.write(hizFonk(ghiz));
  sag_arka_teker.write(hizFonk(hiz));
  sol_on_teker.write(hizFonk(ghiz));
  sol_arka_teker.write(hizFonk(hiz));
  
  }

void sagaDon(){

  sag_on_teker.write(hizFonk(ghiz));
  sag_arka_teker.write(hizFonk(ghiz));
  sol_on_teker.write(hizFonk(hiz));
  sol_arka_teker.write(hizFonk(hiz));
  
  }

void solaDon(){

  sag_on_teker.write(hizFonk(hiz));
  sag_arka_teker.write(hizFonk(hiz));
  sol_on_teker.write(hizFonk(ghiz));
  sol_arka_teker.write(hizFonk(ghiz));
  
  }

void sagaIleriGit(){

//  sag_on_teker.write(hizFonk(ghiz));
  sag_arka_teker.write(hizFonk(ghiz));
  sol_on_teker.write(hizFonk(hiz));
//  sol_arka_teker.write(hizFonk(hiz));
  
  }

void sagaGeriGit(){
  
  sag_on_teker.write(hizFonk(ghiz));
//  sag_arka_teker.write(hizFonk(ghiz));
// sol_on_teker.write(hizFonk(hiz));
  sol_arka_teker.write(hizFonk(ghiz));
  
  }

void solaIleriGit(){
  
  sag_on_teker.write(hizFonk(hiz));
//  sag_arka_teker.write(hizFonk(ghiz));
//  sol_on_teker.write(hizFonk(hiz));
  sol_arka_teker.write(hizFonk(hiz));
  
  }

void solaGeriGit(){

//  sag_on_teker.write(hizFonk(hiz));
  sag_arka_teker.write(hizFonk(ghiz));
  sol_on_teker.write(hizFonk(ghiz));
//  sol_arka_teker.write(hizFonk(hiz));
  
  }

void dur(){
  
  sag_on_teker.write(hizFonk(500));
  sag_arka_teker.write(hizFonk(500));
  sol_on_teker.write(hizFonk(500));
  sol_arka_teker.write(hizFonk(500));
  
  }
