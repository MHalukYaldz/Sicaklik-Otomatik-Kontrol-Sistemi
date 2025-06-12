Bu projede MSP430G2553 mikrodenetleyicisinin ADC portu ile LM35DZ analog sıcaklık sensöründen alınan veri Celcius cinsinden sıcaklık değerine dönüştürülür. Böylece ortam sıcaklığı belirlenir ve kullanıcının istediği sıcaklık değerinin elde edilmesi için soğutucu veya ısıtıcı eleman devreye alınır.

KONTROL KATI

Mikrodenetleyici çalışma gerilimi 3.3V ' tur. Isıtma ve soğutma elemanı için röle modülleri kullanılmıştır. Kullanılan röle modülleri ise 5VDC ile anahtarlanmaktadır. Bundan dolayı röle modüllerinin önüne optokuplörlü bir yapı kullanılmıştır. Hangisinin devrede olduğu optokuplör girişindeki led ile görselleştirilmiştir.
Röle modülleri önündeki S8050 transistörler ile röle anahtarlanıp üzerindeki bobin enerjilendirilmiştir.

REGÜLASYON KATI

12VDC adaptörden alınan giriş gerilimi, röle modülleri ve LCD için L7805CV ile 5VDC gerilimine regüle edilmiştir. Ayrıca 5VDC gerilimi mikrodenetleyicimiz için AMS1117 ile 3.3VDC gerilimine de regüle edilip tüm sistem ayağa kaldırılmıştır.

![PCB_Bottom](https://github.com/user-attachments/assets/f28f9677-de4e-448c-9e76-550b1ea73909)

![PCB_Top](https://github.com/user-attachments/assets/8eb21aa7-ad5f-4c55-bd42-9ff4d33ab3b8)
