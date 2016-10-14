var info, script = {};

(function(){
  $.when($.getJSON('testscript.json')).done(function(data){
    info = data.info;
    script = data.script;
    scriptInit();
  });
}());

function scriptInit(){
  $('#script_title').html('<b>'+info.title+'</b> by'+info.name);
  $('#script_prev').html('(前のシーンが表示されます)').addClass('script0').attr('onclick', 'goPrev();').prop('disabled', true);
  viewScript('#script_now', 0);
  $('#script_now').addClass('script1').prop('disabled', true);
  viewScript('#script_next', 1);
  $('#script_next').addClass('script2').attr('onclick', 'goNext();').prop('disabled', true);
  $('#script_number').html('1/' + script.length);
}

var timer_button = new function(){
  this.start = function(){
    sendComm(0, 0);
    $('#script_next').prop('disabled', false);
    timer = setInterval(function(){pass_time++; readTime();}, 1000);
    $('#timer_start').hide();
    $('#timer_stop').show();
    $('#timer_reset').prop('disabled', true);
  };
  this.stop = function(){
    clearInterval(timer);
    $('#timer_stop').hide();
    $('#timer_restart').show();
    $('#timer_reset').prop('disabled', false);
  };
  this.restart = function(){
    timer = setInterval(function(){pass_time++; readTime();}, 1000);
    $('#timer_restart').hide();
    $('#timer_stop').show();
    $('#timer_reset').prop('disabled', true);
  };
  this.reset = function(){
    pass_time = 0;
    readTime();
    scriptInit();
    $('#timer_restart').hide();
    $('#timer_start').show();
    $('#timer_reset').prop('disabled', true);
  };
  var pass_time = 0;
  var readTime = function(){
    var hour     = toDoubleDigits(Math.floor(pass_time / 3600));
    var minute   = toDoubleDigits(Math.floor((pass_time - 3600*hour) / 60));
    var second   = toDoubleDigits((pass_time - 3600*hour - 60*minute));
    $('#script_timecount').html(hour + ':' + minute + ':' + second);
    return;
  };
  var toDoubleDigits = function(num){return ('0' + num).slice(-2);}; // sliceで時刻要素の0埋め
};

function goNext(){
  $.each(['script_prev', 'script_now', 'script_next'], function(){
    var num = $('#'+this).get(0).className.match(/\d/g).join('') / 1; // 数字だけ取り出して渡す(型変換しないとうまくいかなかった)
    $('#'+this).removeClass($('#'+this).get(0).className).addClass('script' + (num+1));
    if(num+1 > script.length) $('#'+this).html('(原稿の最後です)').prop('disabled', true);
    else{
      if(this == 'script_now') sendComm(num, 0);
      viewScript('#'+this, num);
    }
  });
  $('#script_number').html($('#script_now').get(0).className.match(/\d/g).join('') + '/' + script.length);
}

function goPrev(){
  $.each(['script_prev', 'script_now', 'script_next'], function(){
    var num = $('#'+this).get(0).className.match(/\d/g).join('') / 1; // 数字だけ取り出して渡す(型変換しないとうまくいかなかった)
    $('#'+this).removeClass($('#'+this).get(0).className).addClass('script' + (num-1));
    if(num-1 <= 0) $('#'+this).html('(前のシーンが表示されます)').prop('disabled', true);
    else{
      if(this == 'script_now') sendComm(num-1, 1);
      viewScript('#'+this, num-2);
    }
  });
  $('#script_number').html($('#script_now').get(0).className.match(/\d/g).join('') + '/' + script.length);
}

function sendComm(index, reverse){
  var data = $.extend(true, {}, script[index].projector);
  if(reverse) $.each(data, function(key){
    data[key] = this == 1 ? 0 : 1;
    });
  $.each(ip, function(){
    address = this + 'setPort/status.json';
    getRequest(address, data).done(function(res){checkStatus(res)});
  });
}

function viewScript(id, index){
  if($(id).is(':disabled'))console.log(id); $(id).prop('disabled', false);
  $(id).html(function(){
    var res = ''
    if(!script[index].word) res += '開始直後'
    else {
      res += '「'+script[index].word+'」の';
      switch(script[index].timing){
        case 'pre': res += '前'; break;
        case 'post': res += '後'; break;
        default: res +=  script[index].timing; break;
      }
    }
    $.each(script[index].projector, function(key){
      res += '<br>' + port[key].name + 'を' + (this == 1 ? '点灯' : '消灯');
    });
    return res;
  });
}

