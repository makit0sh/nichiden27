var ip, constellation = [];

function pageInit(){
  xhr = new XMLHttpRequest();
  xhr.addEventListener('loadend', function(){ // xhr.readyStateが4になったら実行される
    if(xhr.status === 200){
      var conf = JSON.parse(xhr.response);
      ip = conf.ip;
      constellation = Object.keys(conf.constellation);
      ip.forEach(function(value){GETRequest(value + 'gpio/0');}); // 点灯状況を読み込む
    }
    else console.error(xhr.status+' '+xhr.statusText);
  });
  xhr.open("GET", "./acrab_conf.json", true); // 設定ファイルを読み込む
  xhr.send();
}

function GETAddress(obj, address){
  if(obj.classList.contains("on")){
    address += 'gpio/0';
  }
  else{
    address += 'gpio/1';
  }
  return address;
}

function GETRequest(address){
  var xhr= new XMLHttpRequest(); // XMLHTTPRequestのインスタンス
  xhr.addEventListener('loadend', function(){ // xhr.readyStateが4になったら実行される
    if(xhr.status === 200){
      console.log(xhr.response);
      checkStatus(JSON.parse(xhr.response)); // checkStatusでボタンのスタイル変更
    }
    else console.error(xhr.status+' '+xhr.statusText);
  });
  xhr.timeout = 5000; // 5秒でタイムアウト
  xhr.open("GET", address);
  xhr.send();
}

function checkStatus(stat){
  constellation.forEach(function(value){
    switch(stat[value]){
      case 1:
        console.log(value + " is on");
        document.getElementById(value).classList.add("on");
        break;
      case 0:
        console.log(value + " is off");
        document.getElementById(value).classList.remove("on");
        break;
    }
  });
}
