import scala.io.StdIn
object PerfectNumber{
  def isPerfect(num: Int): Boolean ={
    var sum = 0
    var i = 1

    while(i < num){
      if(num % i == 0)
        sum += i
      i += 1
    }

    sum == num
  }

  def main(args: Array[String]): Unit ={
    println("Enter 5 numbers:")

    for(i <- 1 to 5){
      val num = StdIn.readInt()
      if(isPerfect(num))
        println(num + " is a Perfect Number")
      else
        println(num + " is Not a Perfect Number")
    }
  }
}