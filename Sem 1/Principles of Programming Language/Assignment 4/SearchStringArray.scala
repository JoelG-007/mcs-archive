import scala.io.StdIn
object SearchStringArray{
  def main(args: Array[String]): Unit ={
    val arr = Array(
      "Scala Programming",
      "Java Programming",
      "Python",
      "Scala",
      "C Programming",
      "Cpp Programming",
      "Machine Learning"
    )

    print("Enter target string: ")
    val target = StdIn.readLine()

    println("Matching Elements:")

    for(item <- arr){
      if (item.contains(target))
        println(item)
    }
  }
}