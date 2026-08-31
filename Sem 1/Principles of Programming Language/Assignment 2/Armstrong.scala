import scala.io.StdIn
object Armstrong{
  def isArmstrong(num: Int): Boolean ={
    var temp = num
    var digits = 0

    while(temp > 0){
      digits += 1
      temp /= 10
    }

    temp = num
    var sum = 0

    while(temp > 0){
      val digit = temp % 10
      sum += Math.pow(digit, digits).toInt
      temp /= 10
    }
    sum == num
  }

  def main(args: Array[String]): Unit ={
    print("Enter a number: ")
    val num = StdIn.readInt()

    if(isArmstrong(num))
      println(num + " is an Armstrong Number")
    else
      println(num + " is Not an Armstrong Number")
  }
}