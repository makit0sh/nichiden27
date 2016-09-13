var ip, constellation,projector = {};

function getIpByConstellation(name){return ip[constellation[name].box];} // target ip address

function pageInit(){ // 読み込み時実行
  /*** 設定ファイル読み込み ***/
  xhr = new XMLHttpRequest();
  xhr.addEventListener('loadend', function(){ // xhr.readyStateが4になったら実行される
    if(xhr.status === 200){
      var conf = JSON.parse(xhr.response);
      ip = conf.ip;
      constellation = conf.constellation;
      projector = conf.projector;
      pinSettingSend();
    }else console.error(xhr.status+' '+xhr.statusText);
  });
  xhr.open("GET", "./acrab_conf.json", true); // 設定ファイルを読み込む
  xhr.send();
}

function pinSettingSend(){
  Object.keys(ip).forEach(function(ipKey){ // モジュールごとに初期設定(pinと星座/投影機の対応)を送信
    var address = ip[ipKey] + 'setConstellationName/status.json?';

    Object.keys(constellation).forEach(function(consKey){ // 星座ごとにpin番号とか見に行く
      if(this[consKey].box != ipKey){return;} // 別のBOX管轄の星座は関係ないので捨てる(高々数十個だし)
      address += this[consKey].pin + '=' + consKey + '&'; // 'p** = ***&'を追加
    },constellation);

    Object.keys(projector).forEach(function(projKey,i){ // 投影機ごとにpin番号とか見に行く
      address += this[projKey].pin + '=' + projKey; // 'p** = ***'を追加
      address += (i===(Object.keys(this).length-1) ? '' : '&'); // 一番最後以外は'&'つける
    },projector);
    console.info('sending initial setting: ' + address);
    getRequest(address);
  });
}

function getAddressFromButton(obj){
  var address = getIpByConstellation(obj.id) + 'setPort/status.json?' + obj.id + '=';
  if(obj.classList.contains("on")) address += 0;
  else address += 1;
  console.debug(address);
  return address;
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
  Object.keys(constellation).forEach(function(value){
    switch(stat[value]){
      case 1:
        console.info(value + " is on");
        document.getElementById(value).classList.add("on");
        break;
      case 0:
        console.info(value + " is off");
        document.getElementById(value).classList.remove("on");
        break;
    }
  });
}
