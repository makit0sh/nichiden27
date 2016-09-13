var ip, port = {};

function pageInit(){ // 読み込み時実行
  /*** 設定ファイル読み込み ***/
  xhr = new XMLHttpRequest();
  xhr.addEventListener('loadend', function(){ // xhr.readyStateが4になったら実行される
    if(xhr.status === 200){
      var conf = JSON.parse(xhr.response);
      ip = conf.ip;
      port = conf.port;
      pinSettingSend(); // pin設定
    }else console.error(xhr.status+' '+xhr.statusText);
  });
  xhr.open("GET", "./acrab_conf.json", true); // 設定ファイルを読み込むリクエスト
  xhr.send();
}

function pinSettingSend(){
  Object.keys(ip).forEach(function(ipKey){ // モジュールごとに初期設定(pinと星座/投影機の対応)を送信
    var address = ip[ipKey] + 'setConstellationName/status.json?';

    Object.keys(port).forEach(function(portKey){ // 星座/投影機ごとにpin番号とか見に行く
      if(this[portKey].box != null && this[portKey].box != ipKey){return;} // 別のBOX管轄の星座は関係ないので捨てる
      address += this[portKey].pin + '=' + portKey + '&'; // 'p** = ***&'を追加
      document.getElementById(portKey).innerText = this[portKey].name; // ついでにボタンに名前をつける
    },port);
    address = address.substr(0,address.length - 1); // 末尾の'&'を削る

    console.info('sending initial setting: ' + address);
    getRequest(address);
  });
}

function requestFromConstellation(obj){
  var address = ip[port[obj.id].box] + 'setPort/status.json?' + obj.id + '=';
  if(obj.classList.contains("on")) address += 0;
  else address += 1;
  console.debug(address);
  getRequest(address);
  return;
}

function requestFromProjector(obj){
  Object.keys(ip).forEach(function(key){
    var address = ip[key] + 'setPort/status.json?' + obj.id + '=';
    if(obj.classList.contains("on")) address += 0;
    else address += 1;
    console.debug(address);
    getRequest(address);
  });
  return;
}

function getRequest(address){
  var xhr= new XMLHttpRequest(); // XMLHTTPRequestのインスタンス
  xhr.addEventListener('loadend', function(){ // xhr.readyStateが4になったら実行される
    if(xhr.status === 200){
      console.debug(xhr.response);
      checkStatus(JSON.parse(xhr.response)); // checkStatusでボタンのスタイル変更
    }
    else console.error(xhr.status+' '+xhr.statusText);
  });
  xhr.timeout = 1000; // 1秒でタイムアウト
  xhr.open("GET", address);
  xhr.send();
}

function checkStatus(stat){
  Object.keys(port).forEach(function(key){
    switch(stat[key]){
      case 1:
        //console.info(key + " is on");
        document.getElementById(key).classList.add("on");
        break;
      case 0:
        //console.info(key + " is off");
        document.getElementById(key).classList.remove("on");
        break;
    }
  });
}
