import scala.io.StdIn
object AverageRange{
  def main(args: Array[String]): Unit ={
    print("Enter first number: ")
    val n1 = StdIn.readInt()
    print("Enter second number: ")
    val n2 = StdIn.readInt()

    var sum = 0
    var count = 0
    var i = n1

    while (i <= n2){
      sum += i
      count += 1
      i += 1
    }
    val average = sum.toDouble / count
    println("Average = " + average)
  }
}