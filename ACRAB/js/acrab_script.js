var info, script = {};

(function(){
  $.when($.getJSON('testscript.json')).done(function(data){
    info = data.info;
    script = data.script;
    $('#script_title').html('<b>'+info.title+'</b> by'+info.name);
    $('#script_prev').html('(前のシーンが表示されます)').addClass('script0').attr('onclick', 'goPrev();');
    viewScript('#script_now', 0);
    $('#script_now').addClass('script1');
    viewScript('#script_next', 1);
    $('#script_next').addClass('script2').attr('onclick', 'goNext();');
  });
}());

function viewScript(id, index){
  if(!$(id).attr('onclick') && id==='#script_prev') $(id).attr('onclick', 'goPrev();');
  else if(!$(id).attr('onclick') && id==='#script_next') $(id).attr('onclick', 'goNext();');
  $(id).html(function(){
    var res = '<b>「'+script[index].word+'」</b>の';
    switch(script[index].timing){
      case 'pre': res += '前'; break;
      case 'post': res += '後'; break;
      default: res +=  script[index].timing; break;
    }
    $.each(script[index].projector, function(key){
      res += '<br>' + port[key].name + 'を' + (this ? '点灯' : '消灯');
    });
    return res;
  });
}

function goNext(){
  $.each(['script_prev', 'script_now', 'script_next'], function(){
    var num = $('#'+this).get(0).className.match(/\d/g).join("") / 1; // 数字だけ取り出して渡す(型変換しないとうまくいかなかった)
    $('#'+this).removeClass($('#'+this).get(0).className).addClass('script' + (num+1));
    if(num+1 > script.length){
      $('#'+this).html('(原稿の最後です)').removeAttr("onclick");
    }else{
      viewScript('#'+this, num);
    }
  });
}

function goPrev(){
  $.each(['script_prev', 'script_now', 'script_next'], function(){
    var num = $('#'+this).get(0).className.match(/\d/g).join("") / 1; // 数字だけ取り出して渡す(型変換しないとうまくいかなかった)
    $('#'+this).removeClass($('#'+this).get(0).className).addClass('script' + (num-1));
    if(num-1 <= 0){
      $('#'+this).html('(前のシーンが表示されます)').removeAttr("onclick");
    }else{
      viewScript('#'+this, num-2);
    }
  });
}

/*

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
}*/
