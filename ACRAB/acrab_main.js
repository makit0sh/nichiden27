var ip, port, group = {};

function pageInit(){ // 読み込み時実行
  /*** 設定ファイル ***/
  $.getJSON("acrab_conf.json", function(data){
    ip = data.ip;
    port = data.port;
    group = data.group;
    $.each($.extend(port,group), function(key){
      $('#'+key).text(this.name); // ボタンに内容を表示する
    });
    pinSettingSend(); // pin設定
  });
}

function pinSettingSend(){
  $.each(ip, function(ipKey){ // モジュールごとに初期設定(pinと星座/投影機の対応)を送信
    var address = this + 'setConstellationName/status.json?';
    $.each(port, function(portKey){ // 星座|投影機ごとにpin番号見に行く
      if(this.box != 'NS' && this.box != ipKey) return; // 別のBOX管轄の星座は関係ないので捨てる
      address += this.pin + '=' + portKey + '&'; // 'p** = ***&'を追加
    });
    address = address.substr(0,address.length - 1); // 末尾の'&'を削る
    console.info('sending initial setting: ' + address);
    getRequest(address);
  });
}

function portStat(obj){return '=' + ($(obj).hasClass('on') ? 0 : 1);}

function requestFromConstellation(obj){
  var address = ip[port[obj.id].box] + 'setPort/status.json?' + obj.id + portStat(obj);
  console.debug(address);
  getRequest(address);
  return;
}

function requestFromProjector(obj){
  $.each(ip, function(){
    var address = this + 'setPort/status.json?' + obj.id + portStat(obj);
    console.debug(address);
    getRequest(address);
  });
  return;
}

function requestFromAll(obj){
  $.each(ip, function(){
    var address = this + (obj.id==='Set' ? 'allSet' : 'allClear') + '/status.json';
    console.debug(address);
    getRequest(address);
  });
  return;
}

function requestFromGroup(obj){
  var address = {}
  $.map(ip, function(value, key){
   address[key] = value + 'setPort/status.json?';
  })
  $.each(group[obj.id].value, function(){ // 各星座ごとに設定を足していく
    if(port[this].box.match('N')) address.N += this + portStat(obj) + '&';
    if(port[this].box.match('S')) address.S += this + portStat(obj) + '&';
  });
  $.each(address, function(key){ // 南北それぞれ送信
    if(this.slice(-1) === '&') address[key] = this.substr(0,this.length - 1); // 末尾が'&'なら削る
    console.debug(address[key]);
    getRequest(address[key]);
  });
}

function getRequest(address){
  $.get({
    url: address, dataType: 'json', timeout: 1000
  }).done(function(data) {
    console.debug(data);
    checkStatus(data); // checkStatusでボタンのスタイル変更
  }).fail(function(xhr) {
    console.error(xhr.status+' '+xhr.statusText);
  });
}

function checkStatus(stat){
  $.each(port, function(key){ // 星座|投影機ごと
    if(stat[key] === 1) $('#'+key).addClass('on');
    else if(stat[key] === 0) $('#'+key).removeClass('on');
  });
  $.each(group, function(key){ // 星座グループごと
    if(!this.value) return;
    var isOn = true;
    $.each(this.value, function(){isOn &= stat[this];}); // 各星座がONかどうかのANDをとる
    if(isOn) $('#'+key).addClass('on');
    else $('#'+key).removeClass('on');
  });
}
