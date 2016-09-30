var ip, port, group = {};

(function(){ // 読み込み時実行
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
}());

$(function() { // ボタンにclickイベントを追加
  $.each($('.main_button'), function(){
      $(this).click(this, function(e){
        if($(e.data).hasClass('constellation')) button.constellation(e.data);
        else if($(e.data).hasClass('projector')) button.projector(e.data);
        else if($(e.data).hasClass('group')) button.group(e.data);
        else if($(e.data).hasClass('all')) button.all(e.data);
      });
  });
});

$(function() { //タブの実装
  $('.tab_area li').click(function() {
    var index = $('.tab_area li').index(this);
    $('.content').css('display','none');
    $('.content').eq(index).css('display','block');
  });
});

function pinSettingSend(){
  $.each(ip, function(ipKey){ // モジュールごとに初期設定(pinと星座/投影機の対応)を送信
    var address = this + 'setConstellationName/status.json?';
    $.each(port, function(portKey){ // 星座|投影機ごとにpin番号見に行く
      if(this.box != 'NS' && this.box != ipKey) return; // 別のBOX管轄の星座は関係ないので捨てる
      address += this.pin + '=' + portKey + '&'; // 'p** = ***&'を追加
    });
    address = address.substr(0,address.length - 1); // 末尾の'&'を削る
    console.info('sending initial setting: ' + address);
    getRequest(address).done(function(res){checkStatus(res)});
  });
  return;
}

function getRequest(address){
  console.debug(address);
  var deferred = $.Deferred(); // 非同期通信なので完了時にデータを渡す処理
  $.get({
    url: address, dataType: 'json', timeout: 1000
  }).done(function(res) {
    console.debug(res);
    deferred.resolve(res);
  }).fail(function(xhr) {
    console.error(xhr.status+' '+xhr.statusText);
    deferred.reject;
  });
  return deferred;
}

function checkStatus(stat){
  $.each(port, function(key){ // 星座|投影機ごと
    if(stat[key] === 1) $('#'+key).addClass('on');
    else if(stat[key] === 0) $('#'+key).removeClass('on');
  });
  $.each(group, function(key){ // 星座グループごと
    if(!this.value) return;
    var isOn = true;
    $.each(this.value, function(){isOn &= $('#'+this).hasClass('on');}); // 各星座がオンかどうかのANDをとる
    if(isOn) $('#'+key).addClass('on');
    else $('#'+key).removeClass('on');
  });
  return;
}


var button = {
  constellation: function(obj){
    var address = ip[port[obj.id].box] + 'setPort/status.json?' + obj.id + button.stat(obj);
    getRequest(address).done(function(res){checkStatus(res)});
    return;
  },
  projector: function(obj){
    $.each(ip, function(){
      var address = this + 'setPort/status.json?' + obj.id + button.stat(obj);
      getRequest(address).done(function(res){checkStatus(res)});
    });
    return;
  },
  all: function(obj){
    $.each(ip, function(){
      var address = this + (obj.id==='Set' ? 'allSet' : 'allClear') + '/status.json';
      getRequest(address).done(function(res){checkStatus(res)});
    });
    return;
  },
  group: function(obj){
    var address = {};
    $.map(ip, function(value, key){
     address[key] = value + 'setPort/status.json?';
    })
    $.each(group[obj.id].value, function(){ // 各(星座|投影機)ごとに設定を足していく
      if(port[this].box.match('N')) address.N += this + button.stat(obj) + '&';
      if(port[this].box.match('S')) address.S += this + button.stat(obj) + '&';
    });
    $.each(address, function(key){ // 南北それぞれ送信
      if(this.slice(-1) === '&') address[key] = this.substr(0,this.length - 1); // 末尾が'&'なら削る
      getRequest(address[key]).done(function(res){checkStatus(res)});
    });
    return;
  },
  stat: function(obj){return '=' + ($(obj).hasClass('on') ? 0 : 1);}
}
