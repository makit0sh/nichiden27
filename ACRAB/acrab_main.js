var ip, port, group = {};

function pageInit(){ // 読み込み時実行
  /*** 設定ファイル読み込み ***/
  xhr = new XMLHttpRequest();
  xhr.addEventListener('loadend', function(){ // xhr.readyStateが4になったら実行される
    if(xhr.status === 200){
      var res = JSON.parse(xhr.response);
      ip = res.ip;
      port = res.port;
      group = res.group;
      buttonInit(); // ボタンに内容を表示する
      pinSettingSend(); // pin設定
    }else console.error(xhr.status+' '+xhr.statusText);
  });
  xhr.open('GET', './acrab_conf.json', true); // 設定ファイルを読み込むリクエスト
  xhr.send();
}

function buttonInit(){
  Object.keys(port).forEach(function(key){ // 星座|投影機ごと
      if(document.getElementById(key)) document.getElementById(key).innerText = this[key].name;
  },port);
  Object.keys(group).forEach(function(key){ // 星座グループごと
      if(document.getElementById(key)) document.getElementById(key).innerText = this[key].name;
  },group);
}

function pinSettingSend(){
  Object.keys(ip).forEach(function(ipKey){ // モジュールごとに初期設定(pinと星座/投影機の対応)を送信
    var address = ip[ipKey] + 'setConstellationName/status.json?';

    Object.keys(port).forEach(function(portKey){ // 星座|投影機ごとにpin番号見に行く
      if(this[portKey].box != null && this[portKey].box != ipKey){return;} // 別のBOX管轄の星座は関係ないので捨てる
      address += this[portKey].pin + '=' + portKey + '&'; // 'p** = ***&'を追加
    },port);
    address = address.substr(0,address.length - 1); // 末尾の'&'を削る

    console.info('sending initial setting: ' + address);
    getRequest(address);
  });
}

function portStat(obj){return '=' + (obj.classList.contains('on') ? 0 : 1);}

function requestFromConstellation(obj){
  var address = ip[port[obj.id].box] + 'setPort/status.json?' + obj.id + portStat(obj);
  console.debug(address);
  getRequest(address);
  return;
}

function requestFromProjector(obj){
  Object.keys(ip).forEach(function(key){
    var address = ip[key] + 'setPort/status.json?' + obj.id + portStat(obj);
    console.debug(address);
    getRequest(address);
  });
  return;
}

function requestFromAll(obj){
  Object.keys(ip).forEach(function(key){
    var address = ip[key] + (obj.id==='Set' ? 'allSet' : 'allClear') + '/status.json';
    console.debug(address);
    getRequest(address);
  });
  return;
}

function requestFromGroup(obj){
  var address = {} // 北天と南天に分かれることがあるので両方用意する
  address.N = ip.N + 'setPort/status.json?';
  address.S = ip.S + 'setPort/status.json?';
  group[obj.id].value.forEach(function(key){ // 各星座ごとに設定を足していく
    if(port[key].box.match('N')) address.N += key + portStat(obj) + '&';
    if(port[key].box.match('S')) address.S += key + portStat(obj) + '&';
  });
  Object.keys(address).forEach(function(key){ // 南北それぞれ送信
    if(this[key].slice(-1) === '?') return; // 設定がなければ送るのやめる
    this[key] = this[key].substr(0,this[key].length - 1); // 末尾の'&'を削る
    console.debug(this[key]);
    getRequest(this[key]);
  },address);
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
  xhr.open('GET', address);
  xhr.send();
}

function checkStatus(stat){
  Object.keys(port).forEach(function(key){ // 星座|投影機ごと
    if(stat[key] && document.getElementById(key)) document.getElementById(key).classList.add('on');
    else if(document.getElementById(key)) document.getElementById(key).classList.remove('on');
  });
  Object.keys(group).forEach(function(groupKey){ // 星座グループごと
    if(!group[groupKey].value) return;
    var isOn = true;
    group[groupKey].value.forEach(function(valueKey){ // 星座ごと
      if(!stat[valueKey]) isOn = false;
    });
    if(isOn) document.getElementById(groupKey).classList.add('on');
    else document.getElementById(groupKey).classList.remove('on');
  });
}
